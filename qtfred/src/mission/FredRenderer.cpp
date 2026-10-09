
#include "FredRenderer.h"
#include "Editor.h"
#include "EditorViewport.h"

#include <globalincs/alphacolors.h>
#include <mission/missiongrid.h>
#include <missioneditor/common.h>
#include <globalincs/systemvars.h>
#include <io/timer.h>
#include <osapi/osapi.h>
#include <io/key.h>
#include <object/object.h>
#include <globalincs/linklist.h>
#include <render/3d.h>
#include <graphics/font.h>
#include <graphics/matrix.h>
#include <graphics/light.h>
#include <lighting/lighting.h>
#include <starfield/starfield.h>
#include <ship/ship.h>
#include <ship/shipfx.h>
#include <coordinate_points/coordinate_point.h>
#include <coordinate_points/coordinate_point_render.h>
#include <jumpnode/jumpnode.h>
#include <asteroid/asteroid.h>
#include <prop/prop.h>
#include <iff_defs/iff_defs.h>
#include <math/fvi.h>
#include <graphics/light.h>
#include <mod_table/mod_table.h>
#include <cfile/cfile.h>
#include <parse/sexp.h>

#include <algorithm>
#include <cmath>

#include "mission/object.h"
#include "prop/prop.h"
#include "weapon/weapon.h"
#include "mission/dialogs/BackgroundEditorDialogModel.h"


namespace {
const float CONVERT_DEGREES = 57.29578f; // conversion factor from radians to degrees

const float FRED_DEAFULT_HTL_DRAW_DIST = 300000.0f;

const int FRED_COLOUR_WHITE = 0xffffff;
const int FRED_COLOUR_YELLOW_GREEN = 0xc8ff00;

// FOV of the viewport being drawn, set at the start of each frame: the GPU projection
// (enable_htl) and the software g3 view (labels, markers, picking) must use the same one
float Fred_frame_fov = FRED_DEFAULT_HTL_FOV;

void enable_htl() {
	gr_set_proj_matrix((4.0f / 9.0f) * PI * Fred_frame_fov,
					   gr_screen.aspect * static_cast<float>(gr_screen.clip_width)
						   / static_cast<float>(gr_screen.clip_height),
					   1.0f,
					   FRED_DEAFULT_HTL_DRAW_DIST);
	gr_set_view_matrix(&Eye_position, &Eye_matrix);
}

void disable_htl() {
	gr_end_proj_matrix();
	gr_end_view_matrix();
}

bool fred_colors_inited = false;
color colour_white;
color colour_green;
color colour_black;
color colour_yellow_green;

void init_fred_colors() {
	if (fred_colors_inited) {
		return;
	}

	fred_colors_inited = true;

	gr_init_alphacolor(&colour_white, 255, 255, 255, 255);
	gr_init_alphacolor(&colour_green, 0, 200, 0, 255);
	gr_init_alphacolor(&colour_yellow_green, 200, 255, 0, 255);
	gr_init_alphacolor(&colour_black, 0, 0, 0, 255);
}

int grid_colors_inited = 0;
color Fred_grid_bright;
color Fred_grid_dark;

// Draws every registered viewport-handle group as small filled screen-space
// squares overlaid on the visualizer. Disabled handles (those whose
// is_enabled callback returns false) render as a dim gray "ghost" so the user
// can see the constraint state without losing track of where the handle is.
//
// Handles carrying an info_label additionally get object-style overlays: a
// persistent name/coordinate line gated on the Show Info / Show Coordinates
// view options, and — when the cursor is over them — the same infobox balloon
// ships show. Asteroid handles leave info_label empty and so render as bare
// markers; only the volumetric gizmo opts in.
void draw_viewport_handles(fso::fred::EditorViewport* viewport,
	const fso::fred::ViewSettings& view,
	fso::fred::EditorViewport::HandlePick hovered) {
	if (!viewport) {
		return;
	}
	const auto& groups = viewport->getHandleGroups();
	if (groups.empty()) {
		return;
	}

	for (size_t gi = 0; gi < groups.size(); ++gi) {
		const auto& group = groups[gi];
		for (size_t hi = 0; hi < group.size(); ++hi) {
			const auto& handle = group[hi];
			vertex vt;
			vec3d pos_copy = handle.world_pos;
			g3_rotate_vertex(&vt, &pos_copy);
			if (vt.codes & CC_BEHIND) {
				continue;
			}
			if (g3_project_vertex(&vt) & PF_OVERFLOW) {
				continue;
			}

			const bool enabled = !handle.is_enabled || handle.is_enabled();
			int r = handle.color_r;
			int g = handle.color_g;
			int b = handle.color_b;
			if (!enabled) {
				// Desaturate + dim. Keeps the handle visible-but-clearly-off.
				r = (r + 128) / 4;
				g = (g + 128) / 4;
				b = (b + 128) / 4;
			}

			// Pick a screen-space size by handle kind. Center handles are
			// larger so they read as "drag the whole thing" at a glance.
			int half;
			switch (handle.kind) {
			case fso::fred::ViewportHandle::Kind::Center: half = 6; break;
			case fso::fred::ViewportHandle::Kind::Corner: half = 5; break;
			case fso::fred::ViewportHandle::Kind::Face:   half = 4; break;
			default:                                      half = 4; break;
			}

			int x = static_cast<int>(vt.screen.xyw.x);
			int y = static_cast<int>(vt.screen.xyw.y);

			// White border for visibility against any background.
			gr_set_color(255, 255, 255);
			gr_rect(x - half - 1, y - half - 1, half * 2 + 3, half * 2 + 3);

			gr_set_color(r, g, b);
			gr_rect(x - half, y - half, half * 2 + 1, half * 2 + 1);

			// A handle surfaces text if it has a name (label) and/or opts into
			// coordinate display. Label follows Show Info; coords follow Show
			// Coordinates. (Asteroid faces/corners are nameless but show_coords.)
			const bool wantsLabel  = view.Show_ship_info && !handle.info_label.empty();
			const bool wantsCoords = view.Show_coordinates && handle.show_coords;

			// Persistent name / coordinate overlay. Green when the owning entity
			// is selected (matching a selected object), white otherwise —
			// deliberately NOT tracking Fred_outline (the selected object's scheme).
			if (wantsLabel || wantsCoords) {
				char buf[256];
				buf[0] = 0;
				if (wantsLabel) {
					strcpy_s(buf, handle.info_label.c_str());
				}
				if (wantsCoords) {
					char pos_str[64];
					sprintf(pos_str, "(%.0f,%.0f,%.0f)", handle.world_pos.xyz.x, handle.world_pos.xyz.y, handle.world_pos.xyz.z);
					if (*buf) {
						strcat_s(buf, "\n");
					}
					strcat_s(buf, pos_str);
				}
				gr_set_color_fast(handle.is_selected ? &colour_green : &colour_white);
				gr_string(x + half + 3, y - half, buf, GR_RESIZE_FULL, view.Label_font_scale);
			}

			// Hover balloon on the handle under the cursor — same infobox ships
			// draw, shown regardless of the Show toggles. Only for handles that
			// carry a name or surface coordinates.
			const bool hovering = hovered.group_index == static_cast<int>(gi) &&
				hovered.handle_index == static_cast<int>(hi);
			if (hovering && (!handle.info_label.empty() || handle.show_coords)) {
				char info[256];
				if (!handle.info_label.empty()) {
					sprintf(info, "%s\n( %.1f , %.1f , %.1f ) ", handle.info_label.c_str(),
						handle.world_pos.xyz.x, handle.world_pos.xyz.y, handle.world_pos.xyz.z);
				} else {
					sprintf(info, "( %.1f , %.1f , %.1f ) ",
						handle.world_pos.xyz.x, handle.world_pos.xyz.y, handle.world_pos.xyz.z);
				}
				int w, h;
				gr_get_string_size(&w, &h, info);
				// scale the box to match the scaled label text, like the object infobox
				w = fl2i(w * view.Label_font_scale);
				h = fl2i(h * view.Label_font_scale);
				int bx = x;
				int by = y + 20;
				gr_set_color_fast(&colour_white);
				gr_rect(bx - 7, by - 6, w + 8, h + 7);
				gr_set_color_fast(&colour_black);
				gr_rect(bx - 5, by - 5, w + 5, h + 5);
				gr_set_color_fast(&colour_white);
				gr_string(bx, by, info, GR_RESIZE_FULL, view.Label_font_scale);
			}
		}
	}
}

void draw_asteroid_field() {
	int i, j;
	vec3d p[8], ip[8];
	vertex v[8], iv[8];

	for (i = 0; i < 1 /*MAX_ASTEROID_FIELDS*/; i++) {
		if (Asteroid_field.num_initial_asteroids) {
			p[0].xyz.x = p[2].xyz.x = p[4].xyz.x = p[6].xyz.x = Asteroid_field.min_bound.xyz.x;
			p[1].xyz.x = p[3].xyz.x = p[5].xyz.x = p[7].xyz.x = Asteroid_field.max_bound.xyz.x;
			p[0].xyz.y = p[1].xyz.y = p[4].xyz.y = p[5].xyz.y = Asteroid_field.min_bound.xyz.y;
			p[2].xyz.y = p[3].xyz.y = p[6].xyz.y = p[7].xyz.y = Asteroid_field.max_bound.xyz.y;
			p[0].xyz.z = p[1].xyz.z = p[2].xyz.z = p[3].xyz.z = Asteroid_field.min_bound.xyz.z;
			p[4].xyz.z = p[5].xyz.z = p[6].xyz.z = p[7].xyz.z = Asteroid_field.max_bound.xyz.z;

			for (j = 0; j < 8; j++) {
				g3_rotate_vertex(&v[j], &p[j]);
			}

			g3_draw_line(&v[0], &v[1]);
			g3_draw_line(&v[2], &v[3]);
			g3_draw_line(&v[4], &v[5]);
			g3_draw_line(&v[6], &v[7]);
			g3_draw_line(&v[0], &v[2]);
			g3_draw_line(&v[1], &v[3]);
			g3_draw_line(&v[4], &v[6]);
			g3_draw_line(&v[5], &v[7]);
			g3_draw_line(&v[0], &v[4]);
			g3_draw_line(&v[1], &v[5]);
			g3_draw_line(&v[2], &v[6]);
			g3_draw_line(&v[3], &v[7]);


			// maybe draw inner box
			if (Asteroid_field.has_inner_bound) {
				gr_set_color(16, 192, 92);

				ip[0].xyz.x = ip[2].xyz.x = ip[4].xyz.x = ip[6].xyz.x = Asteroid_field.inner_min_bound.xyz.x;
				ip[1].xyz.x = ip[3].xyz.x = ip[5].xyz.x = ip[7].xyz.x = Asteroid_field.inner_max_bound.xyz.x;
				ip[0].xyz.y = ip[1].xyz.y = ip[4].xyz.y = ip[5].xyz.y = Asteroid_field.inner_min_bound.xyz.y;
				ip[2].xyz.y = ip[3].xyz.y = ip[6].xyz.y = ip[7].xyz.y = Asteroid_field.inner_max_bound.xyz.y;
				ip[0].xyz.z = ip[1].xyz.z = ip[2].xyz.z = ip[3].xyz.z = Asteroid_field.inner_min_bound.xyz.z;
				ip[4].xyz.z = ip[5].xyz.z = ip[6].xyz.z = ip[7].xyz.z = Asteroid_field.inner_max_bound.xyz.z;

				for (j = 0; j < 8; j++) {
					g3_rotate_vertex(&iv[j], &ip[j]);
				}

				g3_draw_line(&iv[0], &iv[1]);
				g3_draw_line(&iv[2], &iv[3]);
				g3_draw_line(&iv[4], &iv[5]);
				g3_draw_line(&iv[6], &iv[7]);
				g3_draw_line(&iv[0], &iv[2]);
				g3_draw_line(&iv[1], &iv[3]);
				g3_draw_line(&iv[4], &iv[6]);
				g3_draw_line(&iv[5], &iv[7]);
				g3_draw_line(&iv[0], &iv[4]);
				g3_draw_line(&iv[1], &iv[5]);
				g3_draw_line(&iv[2], &iv[6]);
				g3_draw_line(&iv[3], &iv[7]);
			}
		}
	}
}

enum class subsystem_highlight { BOUNDING_BOX, LABEL };

void fredhtl_render_subsystem_highlight(fso::fred::subsys_to_render *s2r, subsystem_highlight highlight, float label_scale = 1.0f)
{
	vertex text_center;
	SCP_string buf;

	auto objp = s2r->ship_obj;
	auto ss = s2r->cur_subsys;

	auto pmi = model_get_instance(Ships[objp->instance].model_instance_num);
	auto pm = model_get(pmi->model_num);
	int subobj_num = ss->system_info->subobj_num;

	auto bsp = &pm->submodel[subobj_num];

	// transform bounding box corners from submodel-local space to world space
	// and draw edges as thick camera-facing quads via g3_render_rod
	color clr_red;
	gr_init_color(&clr_red, 255, 32, 32);
	float rod_width = 2.0f;

	auto transform_and_draw_box = [&](const vec3d *bbox, int sobj_num) {
		vec3d corners[8];
		for (int i = 0; i < 8; i++)
			model_instance_local_to_global_point(&corners[i], &bbox[i], pm, pmi, sobj_num, &objp->orient, &objp->pos);

		// 12 edges of a box: front face, back face, connecting edges
		// bounding_box indices: 0=BBL 1=BBR 2=BTR 3=BTL 4=FBL 5=FBR 6=FTR 7=FTL
		static const int edges[12][2] = {
			{7, 6}, {6, 5}, {5, 4}, {4, 7},  // front face
			{3, 2}, {2, 1}, {1, 0}, {0, 3},  // back face
			{7, 3}, {6, 2}, {4, 0}, {5, 1},  // connecting edges
		};

		for (const auto& edge : edges) {
			vec3d pts[2] = { corners[edge[0]], corners[edge[1]] };
			g3_render_rod(&clr_red, 2, pts, rod_width);
		}
	};

	if (highlight == subsystem_highlight::BOUNDING_BOX) {
		enable_htl();

		// draw a box around the subsystem
		transform_and_draw_box(bsp->bounding_box, subobj_num);

		// draw another box around a gun for a two-part turret
		if ((ss->system_info->turret_gun_sobj >= 0) && (ss->system_info->turret_gun_sobj != ss->system_info->subobj_num))
			transform_and_draw_box(pm->submodel[ss->system_info->turret_gun_sobj].bounding_box, ss->system_info->turret_gun_sobj);

		disable_htl();
	} else {
		// get text
		buf = ss->system_info->subobj_name;

		// add weapons if present
		for (int i = 0; i < ss->weapons.num_primary_banks; ++i)
		{
			int wi = ss->weapons.primary_bank_weapons[i];
			if (wi >= 0)
			{
				buf += "\n";
				buf += Weapon_info[wi].name;
			}
		}
		for (int i = 0; i < ss->weapons.num_secondary_banks; ++i)
		{
			int wi = ss->weapons.secondary_bank_weapons[i];
			if (wi >= 0)
			{
				buf += "\n";
				buf += Weapon_info[wi].name;
			}
		}

		//draw the text.  rotate the center of the subsystem into place before finding out where to put the text
		vec3d center_pt;
		vm_vec_unrotate(&center_pt, &bsp->offset, &objp->orient);
		vm_vec_add2(&center_pt, &objp->pos);
		g3_rotate_vertex(&text_center, &center_pt);
		g3_project_vertex(&text_center);
		if (!(text_center.flags & PF_OVERFLOW)) {
			gr_string_outlined((int)text_center.screen.xyw.x, (int)text_center.screen.xyw.y, buf.c_str(), &colour_white, &colour_black, 2, GR_RESIZE_FULL, label_scale);
		}
	}
}

void render_active_rect(bool box_marking, const fso::fred::Marking_box& marking_box) {
	if (box_marking) {
		gr_set_color(255, 255, 255);
		gr_line(marking_box.x1, marking_box.y1, marking_box.x1, marking_box.y2);
		gr_line(marking_box.x1, marking_box.y2, marking_box.x2, marking_box.y2);
		gr_line(marking_box.x2, marking_box.y2, marking_box.x2, marking_box.y1);
		gr_line(marking_box.x2, marking_box.y1, marking_box.x1, marking_box.y1);
	}
}

void draw_compass_arrow(vec3d* v0) {
	vec3d v1 = vmd_zero_vector;
	vertex tv0, tv1;

	g3_rotate_vertex(&tv0, v0);
	g3_rotate_vertex(&tv1, &v1);
	g3_project_vertex(&tv0);
	g3_project_vertex(&tv1);
	//	tv0.sx = (tv0.sx - tv1.sx) * 1 + tv1.sx;
	//	tv0.sy = (tv0.sy - tv1.sy) * 1 + tv1.sy;
	g3_draw_line(&tv0, &tv1);
}

}

namespace fso::fred {
ViewSettings::ViewSettings() {
}

FredRenderer::FredRenderer(os::Viewport* targetView) : _targetView(targetView) {
	init_fred_colors();
}
FredRenderer::~FredRenderer() {
	freeVolumetricModel();
}
void FredRenderer::setViewport(EditorViewport* viewport) {
	Assertion(_viewport == nullptr, "Resetting viewport is not supported");
	Assertion(viewport != nullptr, "Invalid viewport specified!");

	_viewport = viewport;
}

void FredRenderer::freeVolumetricModel() {
	if (_volumetric_model_num >= 0) {
		model_unload(_volumetric_model_num);
		_volumetric_model_num = -1;
	}
	_volumetric_cached_pof.clear();
}

void FredRenderer::render_grid(grid* gridp) {
	int i, ncols, nrows;

	enable_htl();
	gr_zbuffer_set(0);

	if (!grid_colors_inited) {
		grid_colors_inited = 1;

		gr_init_color(&Fred_grid_dark, 64, 64, 64);
		gr_init_color(&Fred_grid_bright, 128, 128, 128);
	}

	ncols = gridp->ncols;
	nrows = gridp->nrows;
	gr_set_color_fast(&Fred_grid_dark);

	//	Draw the column lines.
	for (i = 0; i <= ncols; i++) {
		g3_draw_htl_line(&gridp->gpoints1[i], &gridp->gpoints2[i]);
	}
	//	Draw the row lines.
	for (i = 0; i <= nrows; i++) {
		g3_draw_htl_line(&gridp->gpoints3[i], &gridp->gpoints4[i]);
	}

	ncols = gridp->ncols / 2;
	nrows = gridp->nrows / 2;

	// now draw the larger, brighter gridlines that is x10 the scale of smaller one.
	gr_set_color_fast(&Fred_grid_bright);

	for (i = 0; i <= ncols; i++) {
		g3_draw_htl_line(&gridp->gpoints5[i], &gridp->gpoints6[i]);
	}

	for (i = 0; i <= nrows; i++) {
		g3_draw_htl_line(&gridp->gpoints7[i], &gridp->gpoints8[i]);
	}

	disable_htl();
	gr_zbuffer_set(1);
}


void FredRenderer::display_distances() {
	char buf[20];
	object *objp, *o2;
	vec3d pos;
	vertex v;


	gr_set_color(255, 0, 0);
	objp = GET_FIRST(&obj_used_list);
	while (objp != END_OF_LIST(&obj_used_list))
	{
		if (objp->flags[Object::Object_Flags::Marked])
		{
			o2 = GET_NEXT(objp);
			while (o2 != END_OF_LIST(&obj_used_list))
			{
				if (o2->flags[Object::Object_Flags::Marked])
				{
					rpd_line(&objp->pos, &o2->pos);
					vm_vec_avg(&pos, &objp->pos, &o2->pos);
					g3_rotate_vertex(&v, &pos);
					if (!(v.codes & CC_BEHIND))
						if (!(g3_project_vertex(&v) & PF_OVERFLOW)) {
							sprintf(buf, "%.1f", vm_vec_dist(&objp->pos, &o2->pos));
							gr_set_color_fast(&colour_white);
							gr_string((int)v.screen.xyw.x, (int)v.screen.xyw.y, buf, GR_RESIZE_FULL, view().Label_font_scale);
						}
				}



				o2 = GET_NEXT(o2);
			}
		}

		objp = GET_NEXT(objp);
	}
}

// A small padlock to the left of a label at (x, y), in the current color, sized with the labels
static void draw_lock_badge(int x, int y, float scale) {
	const int w = std::max(6, fl2i(8.0f * scale));
	const int shackleH = w / 2;
	const int inset = w / 4;
	const int left = x - w - 4;
	// shackle: an upside-down U on top of the body
	gr_line(left + inset, y + shackleH, left + inset, y);
	gr_line(left + inset, y, left + w - 1 - inset, y);
	gr_line(left + w - 1 - inset, y, left + w - 1 - inset, y + shackleH);
	gr_rect(left, y + shackleH, w, w * 3 / 4);
}

void FredRenderer::display_ship_info(int cur_object_index) {
	char buf[512], pos[80];
	int render = 1;
	object* objp;
	vertex v;

	objp = GET_FIRST(&obj_used_list);
	while (objp != END_OF_LIST(&obj_used_list)) {
		Assert(objp->type != OBJ_NONE);
		Fred_outline = 0;
		render = 1;
		if (OBJ_INDEX(objp) == cur_object_index) {
			Fred_outline = FRED_COLOUR_WHITE;
		} else if (objp->flags[Object::Object_Flags::Marked]) { // is it a marked object?
			Fred_outline = FRED_COLOUR_YELLOW_GREEN;
		} else {
			Fred_outline = 0;
		}

		if ((objp->type == OBJ_WAYPOINT) && !view().Show_waypoints) {
			render = 0;
		}

		if ((objp->type == OBJ_START) && !view().Show_starts) {
			render = 0;
		}

		if ((objp->type == OBJ_SHIP) || (objp->type == OBJ_START)) {
			if (!view().Show_ships) {
				render = 0;
			}

			if (!view().Show_iff[Ships[objp->instance].team]) {
				render = 0;
			}
		}

		if ((objp->type == OBJ_PROP) && !view().Show_props) {
			render = 0;
		}

		if ((objp->type == OBJ_JUMP_NODE) && !view().Show_jump_nodes) {
			render = 0;
		}

		if ((objp->type == OBJ_COORDINATE_POINT) && !view().Show_coordinate_points) {
			render = 0;
		}

		if (objp->flags[Object::Object_Flags::Hidden]) {
			render = 0;
		}
		if (!_viewport->isObjectVisibleInLayer(objp)) {
			render = 0;
		}

		g3_rotate_vertex(&v, &objp->pos);
		if (!(v.codes & CC_BEHIND) && render) {
			if (!(g3_project_vertex(&v) & PF_OVERFLOW)) {
				*buf = 0;
				if (view().Show_ship_info) {
					if ((objp->type == OBJ_SHIP) || (objp->type == OBJ_START)) {
						ship* shipp;
						int ship_type;

						shipp = &Ships[objp->instance];
						ship_type = shipp->ship_info_index;
						Assert(ship_type >= 0);
						sprintf(buf, "%s\n%s", shipp->ship_name, Ship_info[ship_type].short_name);
					} else if (objp->type == OBJ_WAYPOINT) {
						int idx;
						waypoint_list* wp_list = find_waypoint_list_with_instance(objp->instance, &idx);
						Assertion(wp_list != nullptr, "Could not find waypoint list for object instance %d", objp->instance);
						if (wp_list == nullptr) {
							objp = GET_NEXT(objp);
							continue;
						}
						sprintf(buf, "%s\nWaypoint %d", wp_list->get_name(), idx + 1);
					} else if (objp->type == OBJ_JUMP_NODE) {
						CJumpNode* jnp = jumpnode_get_by_objnum(OBJ_INDEX(objp));
						sprintf(buf, "%s\n%s", jnp->GetName(), jnp->GetDisplayName());
					} else if (objp->type == OBJ_PROP) {
						auto propp = prop_id_lookup(objp->instance);
						if (propp != nullptr) {
							sprintf(buf, "%s\n", propp->prop_name);
						}
					} else if (objp->type == OBJ_COORDINATE_POINT) {
						auto* cp = find_coordinate_point_by_objnum(OBJ_INDEX(objp));
						if (cp != nullptr) {
							if (!cp->category.empty()) {
								sprintf(buf, "%s\n%s", cp->name.c_str(), cp->category.c_str());
							} else {
								sprintf(buf, "%s", cp->name.c_str());
							}
						}
					} else
						Assert(0);
				}

				if (view().Show_coordinates) {
					sprintf(pos, "(%.0f,%.0f,%.0f)", objp->pos.xyz.x, objp->pos.xyz.y, objp->pos.xyz.z);
					if (*buf)
						strcat_s(buf, "\n");

					strcat_s(buf, pos);
				}

				if (Fred_outline == FRED_COLOUR_WHITE) {
					gr_set_color_fast(&colour_green);
				} else if (Fred_outline == FRED_COLOUR_YELLOW_GREEN) {
					gr_set_color_fast(&colour_yellow_green);
				} else {
					gr_set_color_fast(&colour_white);
				}

				if (*buf) {
					gr_string((int) v.screen.xyw.x, (int) v.screen.xyw.y, buf, GR_RESIZE_FULL, view().Label_font_scale);
				}

				// transform-locked objects get a padlock beside the label; it shows and hides with the labels
				if (view().Show_ship_info && Editor::isTransformLocked(OBJ_INDEX(objp))) {
					draw_lock_badge((int) v.screen.xyw.x, (int) v.screen.xyw.y, view().Label_font_scale);
				}
			}
		}

		objp = GET_NEXT(objp);
	}
}

void FredRenderer::cancel_display_active_ship_subsystem(subsys_to_render& Render_subsys) {
	Render_subsys.do_render = false;
	Render_subsys.ship_obj = NULL;
	Render_subsys.cur_subsys = NULL;
}

void FredRenderer::display_active_ship_subsystem(subsys_to_render& Render_subsys, int cur_object_index) {
	if (cur_object_index != -1) {
		if (Objects[cur_object_index].type == OBJ_SHIP) {
			object* objp = &Objects[cur_object_index];
			if (!_viewport->isObjectVisibleInLayer(objp)) {
				return;
			}

			// if this option is checked, we want to render info for all subsystems, not just the ones we select with K and Shift-K
			if (view().Highlight_selectable_subsys) {
				auto shipp = &Ships[objp->instance];

				// first pass: draw all bounding boxes
				for (auto ss : list_range(&shipp->subsys_list)) {
					if (ss->system_info->subobj_num != -1) {
						subsys_to_render s2r = { true, objp, ss };
						fredhtl_render_subsystem_highlight(&s2r, subsystem_highlight::BOUNDING_BOX);
					}
				}
				// second pass: draw all labels
				for (auto ss : list_range(&shipp->subsys_list)) {
					if (ss->system_info->subobj_num != -1) {
						subsys_to_render s2r = { true, objp, ss };
						fredhtl_render_subsystem_highlight(&s2r, subsystem_highlight::LABEL, view().Label_font_scale);
					}
				}
			}
			// otherwise select individual subsystems, or not, as normal
			else {
				// switching to a new ship, so reset
				if (objp != Render_subsys.ship_obj) {
					cancel_display_active_ship_subsystem(Render_subsys);
					return;
				}

				if (Render_subsys.do_render) {
					fredhtl_render_subsystem_highlight(&Render_subsys, subsystem_highlight::BOUNDING_BOX);
					fredhtl_render_subsystem_highlight(&Render_subsys, subsystem_highlight::LABEL, view().Label_font_scale);
				} else {
					cancel_display_active_ship_subsystem(Render_subsys);
				}
			}
		}
	}
}

void FredRenderer::render_compass() {
	if (!view().Show_compass) {
		return;
	}

	vec3d v, eye = vmd_zero_vector;

	gr_set_clip(gr_screen.max_w - 100, 0, 100, 100);
	g3_start_frame(0); // ** Accounted for
	// required !!!
	vm_vec_scale_add2(&eye, &_viewport->camera.eye_orient.vec.fvec, -1.5f);
	g3_set_view_matrix(&eye, &_viewport->camera.eye_orient, 1.0f);

	v.xyz.x = 1.0f;
	v.xyz.y = v.xyz.z = 0.0f;
	if (vm_vec_dot(&eye, &v) < 0.0f) {
		gr_set_color(159, 20, 20);
	} else {
		gr_set_color(255, 32, 32);
	}
	draw_compass_arrow(&v);

	v.xyz.y = 1.0f;
	v.xyz.x = v.xyz.z = 0.0f;
	if (vm_vec_dot(&eye, &v) < 0.0f) {
		gr_set_color(20, 159, 20);
	} else {
		gr_set_color(32, 255, 32);
	}
	draw_compass_arrow(&v);

	v.xyz.z = 1.0f;
	v.xyz.x = v.xyz.y = 0.0f;
	if (vm_vec_dot(&eye, &v) < 0.0f) {
		gr_set_color(20, 20, 159);
	} else {
		gr_set_color(32, 32, 255);
	}
	draw_compass_arrow(&v);

	g3_end_frame(); // ** Accounted for
}



void FredRenderer::render_model_x_htl(vec3d* pos, grid* gridp, int  /*col_scheme*/) {
	vec3d gpos; //	Location of point on grid.
	vec3d tpos;
	float dxz;
	plane tplane;
	vec3d* gv;

	if (!view().Show_grid_positions) {
		return;
	}

	tplane.A = gridp->gmatrix.vec.uvec.xyz.x;
	tplane.B = gridp->gmatrix.vec.uvec.xyz.y;
	tplane.C = gridp->gmatrix.vec.uvec.xyz.z;
	tplane.D = gridp->planeD;

	compute_point_on_plane(&gpos, &tplane, pos);
	dxz = vm_vec_dist(pos, &gpos) / 8.0f;
	gv = &gridp->gmatrix.vec.uvec;
	if (gv->xyz.x * pos->xyz.x + gv->xyz.y * pos->xyz.y + gv->xyz.z * pos->xyz.z < -gridp->planeD) {
		gr_set_color(0, 127, 0);
	} else {
		gr_set_color(192, 192, 192);
	}


	g3_draw_htl_line(&gpos, pos); //	Line from grid to object center.

	tpos = gpos;

	vm_vec_scale_add2(&gpos, &gridp->gmatrix.vec.rvec, -dxz / 2);
	vm_vec_scale_add2(&gpos, &gridp->gmatrix.vec.fvec, -dxz / 2);

	vm_vec_scale_add2(&tpos, &gridp->gmatrix.vec.rvec, dxz / 2);
	vm_vec_scale_add2(&tpos, &gridp->gmatrix.vec.fvec, dxz / 2);

	g3_draw_htl_line(&gpos, &tpos);

	vm_vec_scale_add2(&gpos, &gridp->gmatrix.vec.rvec, dxz);
	vm_vec_scale_add2(&tpos, &gridp->gmatrix.vec.rvec, -dxz);

	g3_draw_htl_line(&gpos, &tpos);
}

void FredRenderer::render_one_model_htl(object* objp,
										int cur_object_index) {
	int z;
	object* o2;

	Assert(objp->type != OBJ_NONE);
	// OBJ_POINT objects (briefing icons / camera lookat) are a FRED2-era construct.  QtFRED's
	// briefing dialog renders its icons in its own widget and never adds them to the main
	// object list, so encountering one here means something has gone very wrong.
	Assertion(objp->type != OBJ_POINT, "OBJ_POINT object (instance %d) appeared in the main editor's render loop; QtFRED does not support OBJ_POINT objects.", objp->instance);

	// if this object isn't fully created yet, don't render it
	if (objp->type == OBJ_SHIP && Ships[objp->instance].create_time == 0)
		return;
	if (objp->type == OBJ_PROP && (!Props[objp->instance].has_value() || Props[objp->instance].value().create_time == 0))
		return;

	if (objp->type == OBJ_JUMP_NODE) {
		return; // jump nodes have their own render loop in render_frame
	}

	if (objp->type == OBJ_COORDINATE_POINT) {
		return; // coordinate points have their own render loop in render_frame
	}

	if ((objp->type == OBJ_WAYPOINT) && !view().Show_waypoints) {
		return;
	}

	if ((objp->type == OBJ_START) && !view().Show_starts) {
		return;
	}

	if ((objp->type == OBJ_SHIP) || (objp->type == OBJ_START)) {
		if (!view().Show_ships) {
			return;
		}

		if (!view().Show_iff[Ships[objp->instance].team]) {
			return;
		}
	}

	if ((objp->type == OBJ_PROP) && !view().Show_props) {
		return;
	}

	if (objp->flags[Object::Object_Flags::Hidden]) {
		return;
	}

	Fred_outline = 0;

	if (!view().Draw_outlines_on_selected_ships && ((OBJ_INDEX(objp) == cur_object_index) || (objp->flags[Object::Object_Flags::Marked]))) {
		/* don't draw the outlines we would normally draw */;
	} else if (OBJ_INDEX(objp) == cur_object_index) {
		Fred_outline = FRED_COLOUR_WHITE;
	} else if (objp->flags[Object::Object_Flags::Marked]) { // is it a marked object?
		Fred_outline = FRED_COLOUR_YELLOW_GREEN;
	} else if ((objp->type == OBJ_SHIP) && view().Show_outlines) {
		color* iff_color = iff_get_color_by_team_and_object(Ships[objp->instance].team, -1, 1, objp);

		Fred_outline = (iff_color->red << 16) | (iff_color->green << 8) | (iff_color->blue);
	} else if ((objp->type == OBJ_START) && view().Show_outlines) {
		Fred_outline = 0x007f00;
	} else {
		Fred_outline = 0;
	}

	// build flags
	if ((objp->type == OBJ_PROP) && (view().Show_ship_models || view().Show_outlines)) {
		uint64_t flags = MR_NORMAL;

		if (!view().Lighting_on) {
			flags |= MR_NO_LIGHTING;
		}

		if (view().FullDetail) {
			flags |= MR_FULL_DETAIL;
		}

		auto propp = prop_id_lookup(objp->instance);
		if (propp == nullptr || !SCP_vector_inbounds(Prop_info, propp->prop_info_index)) {
			return;
		}

		model_render_params render_info;
		render_info.set_debug_flags(0);

		if (Fred_outline) {
			// use a different LOD for the wireframe to reduce visual clutter on high-poly models
			int prop_model_num = Prop_info[propp->prop_info_index].model_num;
			int outline_lod = std::min(view().Outline_lod, model_get(prop_model_num)->n_detail_levels - 1);
			render_info.set_detail_level_lock(outline_lod);
			render_info.set_color(Fred_outline >> 16, (Fred_outline >> 8) & 0xff, Fred_outline & 0xff);
			render_info.set_flags(flags | MR_SHOW_OUTLINE_HTL | MR_NO_LIGHTING | MR_NO_POLYS | MR_NO_TEXTURING);
			model_render_immediate(&render_info,
								   prop_model_num,
								   propp->model_instance_num,
								   &objp->orient,
								   &objp->pos);
			render_info.set_detail_level_lock(-1);
		}

		render_info.set_flags(flags);
		auto* prop_pmi = model_get_instance(propp->model_instance_num);
		render_info.set_replacement_textures(prop_pmi ? prop_pmi->texture_replace : nullptr);
		model_render_immediate(&render_info,
							   Prop_info[propp->prop_info_index].model_num,
							   propp->model_instance_num,
							   &objp->orient,
							   &objp->pos);
	} else if ((view().Show_ship_models || view().Show_outlines) && ((objp->type == OBJ_SHIP) || (objp->type == OBJ_START))) {
		uint64_t flags = 0;

		g3_start_instance_matrix(&Eye_position, &Eye_matrix, 0);
		if (view().Show_ship_models) {
			flags = MR_NORMAL;
		} else {
			flags = MR_NO_POLYS;
		}

		uint debug_flags = 0;
		if (view().Show_dock_points) {
			debug_flags |= MR_DEBUG_DOCK_POINTS;
		}

		if (view().Show_bay_paths) {
			debug_flags |= MR_DEBUG_BAY_PATHS;
		}

		if (view().Show_paths_fred) {
			debug_flags |= MR_DEBUG_PATHS;
		}

		z = objp->instance;

		model_clear_instance(Ship_info[Ships[z].ship_info_index].model_num);

		if (!view().Lighting_on) {
			flags |= MR_NO_LIGHTING;
		}

		if (view().FullDetail) {
			flags |= MR_FULL_DETAIL;
		}

		g3_done_instance(false);

		int ship_model_num = Ship_info[Ships[z].ship_info_index].model_num;
		int ship_model_instance_num = Ships[z].model_instance_num;

		// Outline pass: use a dedicated pass with MR_NO_POLYS so is_outlines_only_htl fires
		// in the renderer. Modern HTL models don't have outline_buffer, so relying on
		// MR_SHOW_OUTLINE_HTL alone (without MR_NO_POLYS) silently does nothing.
		if (Fred_outline) {
			model_render_params outline_info;
			// use a different LOD for the wireframe to reduce visual clutter on high-poly models
			int outline_lod = std::min(view().Outline_lod, model_get(ship_model_num)->n_detail_levels - 1);
			outline_info.set_detail_level_lock(outline_lod);

			outline_info.set_color(Fred_outline >> 16, (Fred_outline >> 8) & 0xff, Fred_outline & 0xff);
			outline_info.set_flags(flags | MR_SHOW_OUTLINE_HTL | MR_NO_POLYS | MR_NO_LIGHTING | MR_NO_TEXTURING);
			model_render_immediate(&outline_info, ship_model_num, ship_model_instance_num, &objp->orient, &objp->pos);

			outline_info.set_detail_level_lock(-1);
		}

		if (view().Show_ship_models) {
			model_render_params render_info;
			render_info.set_debug_flags(debug_flags);
			render_info.set_replacement_textures(model_get_instance(ship_model_instance_num)->texture_replace);
			render_info.set_flags(flags);
			if (Ship_info[Ships[z].ship_info_index].uses_team_colors)
				render_info.set_team_color(Ships[z].team_name, Ships[z].secondary_team_name, Ships[z].team_change_timestamp, Ships[z].team_change_time);
			model_render_immediate(&render_info, ship_model_num, ship_model_instance_num, &objp->orient, &objp->pos);
		}

		if (view().Draw_outline_at_warpin_position
			&& (Ships[z].arrival_cue != Locked_sexp_true || Ships[z].arrival_delay > 0)
			&& Ships[z].arrival_cue != Locked_sexp_false
			&& !Ships[z].flags[Ship::Ship_Flags::No_arrival_warp])
		{
			int warp_type = Warp_params[Ships[z].warpin_params_index].warp_type;
			if (warp_type == WT_DEFAULT || warp_type == WT_KNOSSOS || warp_type == WT_DEFAULT_THEN_KNOSSOS || (warp_type & WT_DEFAULT_WITH_FIREBALL)) {
				float warpin_dist = shipfx_calculate_arrival_warp_distance(objp);

				// project the ship forward as far as it should go
				vec3d warpin_pos;
				vm_vec_scale_add(&warpin_pos, &objp->pos, &objp->orient.vec.fvec, warpin_dist);

				model_render_params warpin_info;
				warpin_info.set_color(65, 65, 65);	// grey; see rgba_defaults
				warpin_info.set_flags(flags | MR_SHOW_OUTLINE_HTL | MR_NO_LIGHTING | MR_NO_POLYS | MR_NO_TEXTURING);
				model_render_immediate(&warpin_info, Ship_info[Ships[z].ship_info_index].model_num, Ships[z].model_instance_num, &objp->orient, &warpin_pos);
			}
		}
	} else {
		int r = 0, g = 0, b = 0;

		if (objp->type == OBJ_SHIP) {
			if (!view().Show_ships) {
				return;
			}

			color* iff_color = iff_get_color_by_team_and_object(Ships[objp->instance].team, -1, 1, objp);

			r = iff_color->red;
			g = iff_color->green;
			b = iff_color->blue;
		} else if (objp->type == OBJ_START) {
			r = 0;
			g = 127;
			b = 0;
		} else if (objp->type == OBJ_WAYPOINT) {
			waypoint_list* wpt_list = find_waypoint_list_with_instance(objp->instance);
			if (wpt_list && wpt_list->get_has_custom_color()) {
				r = wpt_list->get_color_r();
				g = wpt_list->get_color_g();
				b = wpt_list->get_color_b();
			} else {
				r = 96;
				g = 0;
				b = 112;
			}
		} else if (objp->type == OBJ_PROP) {
			r = 255;
			g = 255;
			b = 255;
		} else
			Assert(0);

		float size = fl_sqrt(vm_vec_dist(&_viewport->camera.eye_pos, &objp->pos) / 20.0f);

		if (size < LOLLIPOP_SIZE) {
			size = LOLLIPOP_SIZE;
		}

		if (Fred_outline) {
			gr_set_color(std::min(r * 2, 255), std::min(g * 2, 255), std::min(b * 2, 255));
			g3_draw_htl_sphere(&objp->pos, size * 1.5f);
		} else {
			gr_set_color(r, g, b);
			g3_draw_htl_sphere(&objp->pos, size);
		}
	}

	if (objp->type == OBJ_WAYPOINT) {
		waypoint_list* wpt_list = find_waypoint_list_with_instance(objp->instance);
		if (!wpt_list || !wpt_list->get_no_draw_lines()) {
			for (auto objIdx : rendering_order) {
				o2 = &Objects[objIdx];
				if (o2->type == OBJ_WAYPOINT) {
					if ((o2->instance == objp->instance - 1) || (o2->instance == objp->instance + 1)) {
						g3_draw_htl_line(&o2->pos, &objp->pos);
					}
				}
			}
		}
	}

	render_model_x_htl(&objp->pos, _viewport->The_grid);
	rendering_order.push_back(OBJ_INDEX(objp));
}

void FredRenderer::draw_background_handles()
{
	auto* model = _viewport->backgroundEditModel();
	if (model == nullptr) {
		return;
	}

	// Handles are only meaningful when the background is actually drawn.
	if (!view().Show_stars) {
		return;
	}

	// The element getters read global background state, so they're static; only
	// the current selection is per-instance and comes through the pointer.
	using BgModel = dialogs::BackgroundEditorDialogModel;
	const int selectedSun = model->getSelectedSunIndex();
	const int selectedBitmap = model->getSelectedBitmapIndex();

	auto draw_one = [&](bool isSun, int index, bool selected) {
		vec3d dir;
		const bool ok = isSun ? BgModel::getSunDirection(index, dir)
		                      : BgModel::getBitmapDirection(index, dir);
		if (!ok) {
			return;
		}

		vec3d world;
		vm_vec_scale_add(&world, &_viewport->camera.eye_pos, &dir, BG_HANDLE_DISTANCE);

		vertex vt;
		g3_rotate_vertex(&vt, &world);
		if (vt.codes & CC_BEHIND) {
			return;
		}
		if (g3_project_vertex(&vt) & PF_OVERFLOW) {
			return;
		}

		const int sx = static_cast<int>(vt.screen.xyw.x);
		const int sy = static_cast<int>(vt.screen.xyw.y);
		const int r = selected ? 12 : 7;

		if (selected) {
			gr_set_color(255, 255, 0);   // highlight
		} else if (isSun) {
			gr_set_color(255, 200, 0);   // suns: amber
		} else {
			gr_set_color(0, 200, 255);   // bitmaps: cyan
		}

		// unfilled box around the element's direction
		gr_line(sx - r, sy - r, sx + r, sy - r);
		gr_line(sx + r, sy - r, sx + r, sy + r);
		gr_line(sx + r, sy + r, sx - r, sy + r);
		gr_line(sx - r, sy + r, sx - r, sy - r);

		if (selected) {
			// crosshair ticks + name label so the active element stands out
			gr_line(sx - r - 5, sy, sx - r, sy);
			gr_line(sx + r, sy, sx + r + 5, sy);
			gr_line(sx, sy - r - 5, sx, sy - r);
			gr_line(sx, sy + r, sx, sy + r + 5);

			const SCP_string name = isSun ? BgModel::getSunNameAt(index) : BgModel::getBitmapNameAt(index);
			if (!name.empty()) {
				int w, h;
				gr_get_string_size(&w, &h, name.c_str());
				gr_set_color_fast(&colour_white);
				gr_string(sx - w / 2, sy + r + 6, name.c_str());
			}
		}
	};

	// Draw unselected handles first, then the selected one on top.
	const int sunCount = BgModel::getSunCount();
	const int bitmapCount = BgModel::getBitmapCount();

	for (int i = 0; i < sunCount; i++) {
		if (i != selectedSun) {
			draw_one(true, i, false);
		}
	}
	for (int i = 0; i < bitmapCount; i++) {
		if (i != selectedBitmap) {
			draw_one(false, i, false);
		}
	}
	if (selectedSun >= 0) {
		draw_one(true, selectedSun, true);
	}
	if (selectedBitmap >= 0) {
		draw_one(false, selectedBitmap, true);
	}
}

void FredRenderer::render_volumetric_overlay() {
	if (!The_mission.volumetrics) {
		return;
	}

	constexpr float alpha = 0.35f;

	const volumetric_nebula& neb = *The_mission.volumetrics;
	if (!neb.get_enabled() || neb.getHullPof().empty()) {
		return;
	}

	const SCP_string& pof = neb.getHullPof();
	if (pof != _volumetric_cached_pof) {
		// Stamp the cache before model_load so a re-entrant paint (e.g. from
		// an Error() dialog pumping events on a missing POF) sees the load
		// as already attempted and bails out instead of re-loading.
		freeVolumetricModel();
		_volumetric_cached_pof = pof;
		if (cf_exists_full(pof.c_str(), CF_TYPE_MODELS)) {
			_volumetric_model_num = model_load(pof.c_str());
		} else {
			mprintf(("Volumetric nebula hull POF '%s' not found; skipping editor overlay.\n", pof.c_str()));
			if (_viewport->dialogProvider != nullptr) {
				SCP_string msg = "Volumetric nebula hull POF '";
				msg += pof;
				msg += "' was not found. The nebula will render without an editor overlay until a valid POF is set in the Volumetric Nebula dialog.";
				_viewport->dialogProvider->showButtonDialog(DialogType::Warning,
															"Volumetric Nebula POF Missing",
															msg,
															{ DialogButton::Ok });
			}
		}
	}

	if (_volumetric_model_num < 0) {
		return;
	}

	const auto& col = neb.getNebulaColor();
	// Premultiply by alpha. MR_NO_TEXTURING + MR_ALL_XPARENT lands on
	// ALPHA_BLEND_ADDITIVE (glBlendFunc(GL_ONE, GL_ONE)) which ignores
	// src.alpha, so scaling RGB here is what actually controls intensity.
	const int r = static_cast<int>(std::get<0>(col) * alpha * 255.0f);
	const int g = static_cast<int>(std::get<1>(col) * alpha * 255.0f);
	const int b = static_cast<int>(std::get<2>(col) * alpha * 255.0f);
	const vec3d pos = neb.getPos();

	enable_htl();

	model_render_params fill;
	fill.set_color(r, g, b);
	fill.set_alpha(1.0f);
	fill.set_flags(MR_NO_LIGHTING | MR_NO_TEXTURING | MR_NO_BATCH | MR_ALL_XPARENT);
	model_render_immediate(&fill, _volumetric_model_num, &vmd_identity_matrix, &pos);

	disable_htl();
}

void FredRenderer::render_camera_gizmo() {
	// Looking through the camera, the frustum would only be in the way
	if (!view().Show_camera_gizmo || _viewport->camera.getViewpoint() == EditorViewport::CutsceneCameraViewpoint) {
		return;
	}
	CameraSexpPreview preview;
	if (!_viewport->cameraPreview(&preview)) {
		return;
	}

	// The selected sexp's own pose, when a later sexp of its shot overrides it: drawn faintly,
	// so you can see what it does before it is overridden. Not while playing.
	const bool ghost = preview.hasOwnShot && !_viewport->cameraPlaybackActive();
	CameraShot shown = preview.shot;
	CameraShot own = preview.ownShot;

	// Follow a handle drag before it is written, on the selected sexp's own pose: the camera
	// moves, or turns to the new point
	vec3d dragged;
	if (_viewport->cameraDragPoint(&dragged)) {
		CameraShot& target = ghost ? own : shown;
		if (preview.op == OP_CUTSCENES_SET_CAMERA_POSITION) {
			target.pos = dragged;
		} else if (!vm_vec_same(&dragged, &target.pos)) {
			vec3d dir;
			vm_vec_normalized_dir(&dir, &dragged, &target.pos);
			vm_vector_2_matrix_norm(&target.orient, &dir, nullptr, nullptr);
			target.hasAimPoint = true;
			target.aimPoint = dragged;
		}
	}

	auto line = [](const vec3d& a, const vec3d& b) {
		vertex va, vb;
		g3_rotate_vertex(&va, &a);
		g3_rotate_vertex(&vb, &b);
		g3_draw_line(&va, &vb);
	};

	// The shape the game would show: the vertical angle is zoom * PROJ_FOV_FACTOR, and the
	// horizontal follows the screen's shape (this viewport's, as the look-through view uses it)
	const float screenW = i2fl(std::max(gr_screen.clip_width, 1));
	const float screenH = i2fl(std::max(gr_screen.clip_height, 1));
	auto drawFrustum = [&](const CameraShot& shot, int r, int g, int b, int aimR, int aimG, int aimB) {
		const vec3d& apex = shot.pos;
		const matrix& orient = shot.orient;
		// A fixed share of the distance from the eye, so it reads the same size from anywhere
		const float len = std::max(vm_vec_dist(&apex, &_viewport->camera.eye_pos) * 0.12f, 5.0f);
		const float fov = std::clamp(shot.fov, 0.05f, 2.2f);
		const float halfH = len * tanf(fov * PROJ_FOV_FACTOR * 0.5f);
		const float halfW = halfH * screenW / screenH;

		vec3d center;
		vm_vec_scale_add(&center, &apex, &orient.vec.fvec, len);
		vec3d corners[4];
		const float sx[4] = {-1.0f, 1.0f, 1.0f, -1.0f};
		const float sy[4] = {1.0f, 1.0f, -1.0f, -1.0f};
		for (int i = 0; i < 4; ++i) {
			vm_vec_scale_add(&corners[i], &center, &orient.vec.rvec, sx[i] * halfW);
			vm_vec_scale_add2(&corners[i], &orient.vec.uvec, sy[i] * halfH);
		}

		gr_set_color(r, g, b);
		for (int i = 0; i < 4; ++i) {
			line(apex, corners[i]);
			line(corners[i], corners[(i + 1) % 4]);
		}

		// Which way is up, for the roll
		vec3d top, tipL, tipR;
		vm_vec_scale_add(&top, &center, &orient.vec.uvec, halfH * 1.4f);
		vm_vec_scale_add(&tipL, &center, &orient.vec.uvec, halfH);
		vm_vec_scale_add2(&tipL, &orient.vec.rvec, -halfW * 0.25f);
		vm_vec_scale_add(&tipR, &center, &orient.vec.uvec, halfH);
		vm_vec_scale_add2(&tipR, &orient.vec.rvec, halfW * 0.25f);
		line(tipL, top);
		line(top, tipR);

		if (shot.hasAimPoint) {
			gr_set_color(aimR, aimG, aimB);
			line(apex, shot.aimPoint);
		}
	};

	if (ghost) {
		drawFrustum(own, 50, 95, 115, 35, 65, 80);
	}
	drawFrustum(shown, 0, 200, 255, 0, 120, 160);
	if (shown.hostObj >= 0) {
		gr_set_color(0, 200, 120);
		line(shown.pos, Objects[shown.hostObj].pos);
	}
}

void FredRenderer::render_models(int cur_object_index) {
	gr_set_color_fast(&colour_white);

	rendering_order.clear();

	if ((ENVMAP == -1) && strlen(The_mission.envmap_name)) {
		ENVMAP = bm_load(The_mission.envmap_name);
	}

	bool f = false;
	enable_htl();

	auto render_function = [&](object* objp) {
		if (!_viewport->isObjectVisibleInLayer(objp)) {
			return;
		}
		this->render_one_model_htl(objp, cur_object_index);
	};

	obj_render_all(render_function, &f);

	disable_htl();
}

void FredRenderer::render_frame(int cur_object_index,
	subsys_to_render& Render_subsys,
	bool box_marking,
	const Marking_box& marking_box,
	qreal scale)
{

	// Make sure our OpenGL context is used for rendering
	gr_use_viewport(_targetView);
	uint32_t width = _targetView->getSize().first * scale;
	uint32_t height = _targetView->getSize().second * scale;
	// Resize the rendering window in case the previous size was different
	gr_screen_resize(width, height);

	char buf[256];
	int x, y, w, h, inst;
	vec3d pos;
	vertex v;
	angles a, a_deg; //a is in rads, a_deg is in degrees

	if (g3_in_frame())
		g3_end_frame(); // ** Accounted for

	gr_reset_clip();
	gr_clear();

	g3_start_frame(1); // ** Accounted for
	// 1 means use zbuffering

	font::set_font(font::FONT1);
	light_reset();

	Fred_frame_fov = _viewport->viewFov();
	g3_set_view_matrix(&_viewport->camera.eye_pos, &_viewport->camera.eye_orient, Fred_frame_fov);

	// Force max star detail so the editor always shows the full Num_stars count
	// regardless of the player's graphics quality setting (Detail.num_stars can be 0).
	int saved_detail_stars = Detail.num_stars;
	Detail.num_stars = MAX_DETAIL_VALUE;
	enable_htl();
	stars_draw(view().Show_stars, view().Show_stars, view().Show_stars, 0, 0);
	disable_htl();
	Detail.num_stars = saved_detail_stars;

	if (view().Show_horizon) {
		gr_set_color(128, 128, 64);
		g3_draw_horizon_line();
	}

	// Environment visibility (Scene Browser "Environment" toggle) hides the
	// asteroid-field wireframe and the volumetric hull, alongside their gizmos.
	const bool showEnv = _viewport->editor == nullptr || _viewport->editor->showEnvironment();

	if (showEnv) {
		gr_set_color(192, 96, 16);
		draw_asteroid_field();
	}

	if (view().Show_grid) {
		render_grid(_viewport->The_grid);
	}

	gr_set_color(0, 0, 64);
	render_models(cur_object_index);
	if (showEnv) {
		render_volumetric_overlay();
	}

	// Draw coordinate-point shapes before the text overlays so the per-object label (name,
	// group, coords) lands ON TOP of the shape rather than getting covered by it.
	if (view().Show_coordinate_points) {
		enable_htl();
		for (const auto& cp : Coordinate_points) {
			if (cp.objnum < 0) {
				continue;
			}
			if (!_viewport->isObjectVisibleInLayer(&Objects[cp.objnum])) {
				continue;
			}
			// Grid-position stalk (line to the grid plus the X), like every other
			// object; drawn first so the shape sits on top. Self-gates on
			// Show_grid_positions.
			render_model_x_htl(&Objects[cp.objnum].pos, _viewport->The_grid);
			draw_coordinate_point_shape(cp, &_viewport->camera.eye_pos, &_viewport->camera.eye_orient);
		}
		disable_htl();
	}

	// Keep the always-on gizmos in sync with the mission before they are
	// drawn/picked (cheap no-ops unless the underlying state changed).
	_viewport->refreshVolumetricHandle();
	_viewport->refreshAsteroidHandles();
	_viewport->refreshCameraHandle();

	// Grid-position indicators for any handle that opts in (volumetric center,
	// asteroid box centers), drawn like an object's so height above/below the
	// grid plane is readable. render_model_x_htl self-gates on
	// Show_grid_positions; the enable_htl bracket is needed because
	// render_models() left HTL disabled.
	if (view().Show_grid_positions) {
		enable_htl();
		for (const auto& group : _viewport->getHandleGroups()) {
			for (const auto& handle : group) {
				if (handle.show_grid_position) {
					vec3d hp = handle.world_pos;
					render_model_x_htl(&hp, _viewport->The_grid);
				}
			}
		}
		disable_htl();
	}

	render_camera_gizmo();

	// Viewport handles overlay every visualizer (asteroid box, volumetric
	// hull) and need to draw after them so the markers sit on top of the
	// wireframe and the translucent hull.
	draw_viewport_handles(_viewport, view(), _viewport->getHoveredHandle());

	if (view().Show_distances) {
		display_distances();
	}

	display_ship_info(cur_object_index);
	display_active_ship_subsystem(Render_subsys, cur_object_index);
	render_active_rect(box_marking, marking_box);

	if (query_valid_object(_viewport->Cursor_over) && _viewport->isObjectVisibleInLayer(&Objects[_viewport->Cursor_over])) { // display a tool-tip like infobox
		pos = Objects[_viewport->Cursor_over].pos;
		inst = Objects[_viewport->Cursor_over].instance;
		if ((Objects[_viewport->Cursor_over].type == OBJ_SHIP) || (Objects[_viewport->Cursor_over].type == OBJ_START)) {
			vm_extract_angles_matrix(&a, &Objects[_viewport->Cursor_over].orient);

			a_deg.h = a.h * CONVERT_DEGREES; // convert angles to more readable degrees
			a_deg.p = a.p * CONVERT_DEGREES;
			a_deg.b = a.b * CONVERT_DEGREES;

			sprintf(buf,
					"%s\n%s\n( %.1f , %.1f , %.1f ) \nPitch: %.2f\nBank: %.2f\nHeading: %.2f",
					Ships[inst].ship_name,
					Ship_info[Ships[inst].ship_info_index].short_name,
					pos.xyz.x,
					pos.xyz.y,
					pos.xyz.z,
					a_deg.p,
					a_deg.b,
					a_deg.h);
		} else if (Objects[_viewport->Cursor_over].type == OBJ_WAYPOINT) {
			int idx;
			waypoint_list* wp_list = find_waypoint_list_with_instance(inst, &idx);
			Assertion(wp_list != nullptr, "Could not find waypoint list for object instance %d", inst);
			if (wp_list == nullptr) {
				sprintf(buf, "Waypoint %d\n( %.1f , %.1f , %.1f ) ", idx + 1, pos.xyz.x, pos.xyz.y, pos.xyz.z);
			} else {
				sprintf(buf,
						"%s\nWaypoint %d\n( %.1f , %.1f , %.1f ) ",
						wp_list->get_name(),
						idx + 1,
						pos.xyz.x,
						pos.xyz.y,
						pos.xyz.z);
			}
		} else {
			sprintf(buf, "( %.1f , %.1f , %.1f ) ", pos.xyz.x, pos.xyz.y, pos.xyz.z);
		}

		g3_rotate_vertex(&v, &pos);
		if (!(v.codes & CC_BEHIND)) {
			if (!(g3_project_vertex(&v) & PF_OVERFLOW)) {
				gr_get_string_size(&w, &h, buf);
				// scale the box to match the scaled label text
				w = fl2i(w * view().Label_font_scale);
				h = fl2i(h * view().Label_font_scale);

				x = (int) v.screen.xyw.x;
				y = (int) v.screen.xyw.y + 20;

				gr_set_color_fast(&colour_white);
				gr_rect(x - 7, y - 6, w + 8, h + 7);

				gr_set_color_fast(&colour_black);
				gr_rect(x - 5, y - 5, w + 5, h + 5);

				gr_set_color_fast(&colour_white);
				gr_string(x, y, buf, GR_RESIZE_FULL, view().Label_font_scale);
			}
		}
	}

	gr_set_color(0, 160, 0);

	enable_htl();
	if (view().Show_jump_nodes) {
		for (auto& jn : Jump_nodes) {
			const object* jnObj = jn.GetSCPObject();
			if (jnObj != nullptr && _viewport->isObjectVisibleInLayer(jnObj)) {
				jn.Render(&jnObj->pos);
			}
		}
	}
	disable_htl();

	sprintf(buf, "(%.1f,%.1f,%.1f)", _viewport->camera.eye_pos.xyz.x, _viewport->camera.eye_pos.xyz.y, _viewport->camera.eye_pos.xyz.z);
	gr_get_string_size(&w, &h, buf);
	w = fl2i(w * view().Label_font_scale);
	gr_set_color_fast(&colour_white);
	gr_string(gr_screen.max_w - w - 2, 2, buf, GR_RESIZE_FULL, view().Label_font_scale);

	const auto hiddenLayerCount = _viewport->getHiddenLayerCount();
	if (hiddenLayerCount > 0) {
		gr_set_color(255, 0, 0);
		sprintf(buf, "%d %s Hidden",
				hiddenLayerCount,
				hiddenLayerCount == 1 ? "Layer" : "Layers");
		gr_string(8, 8, buf, GR_RESIZE_FULL, view().Label_font_scale);
	}

	draw_background_handles();

	g3_end_frame(); // ** Accounted for
	render_compass();

	gr_flip();

	gr_reset_clip();

	g3_start_frame(0); // ** Accounted for
	g3_set_view_matrix(&_viewport->camera.eye_pos, &_viewport->camera.eye_orient, Fred_frame_fov);
}
void FredRenderer::resize(int width, int height) {
	// Make sure the following call targets the right view port
	gr_use_viewport(_targetView);

	gr_screen_resize(width, height);

	// We need to rerender the scene now
	scheduleUpdate();
}
ViewSettings& FredRenderer::view() {
	return _viewport->view;
}

} // namespace fso::fred
