// captureState() and restoreState() for TeamLoadoutDialogModel.
// Snapshots Team_data[] (ship/weapon pool lists and required flags) so that
// undo/redo can restore the loadout to its pre-accept state.

#include <mission/dialogs/TeamLoadoutDialogModel.h>

#include <globalincs/globals.h>
#include <mission/missionparse.h>

#include <QByteArray>
#include <QDataStream>
#include <QIODevice>

namespace fso::fred::dialogs {

QByteArray TeamLoadoutDialogModel::captureState() const
{
	QByteArray data;
	QDataStream ds(&data, QIODevice::WriteOnly);

	ds << static_cast<qint32>(Num_teams);

	for (int t = 0; t < Num_teams; ++t) {
		const team_data& td = Team_data[t];

		// Ships
		ds << static_cast<qint32>(td.num_ship_choices);
		for (int i = 0; i < td.num_ship_choices; ++i) {
			ds << static_cast<qint32>(td.ship_list[i]);
			ds << QString::fromLatin1(td.ship_list_variables[i]);
			ds << static_cast<qint32>(td.ship_count[i]);
			ds << QString::fromLatin1(td.ship_count_variables[i]);
		}

		// Weapons
		ds << static_cast<qint32>(td.num_weapon_choices);
		ds << static_cast<quint8>(td.do_not_validate ? 1 : 0);
		for (int i = 0; i < td.num_weapon_choices; ++i) {
			ds << static_cast<qint32>(td.weaponry_pool[i]);
			ds << QString::fromLatin1(td.weaponry_pool_variable[i]);
			ds << static_cast<qint32>(td.weaponry_count[i]);
			ds << QString::fromLatin1(td.weaponry_amount_variable[i]);
		}

		// weapon_required is indexed by weapon class index, not pool position
		ds << static_cast<qint32>(MAX_WEAPON_TYPES);
		for (const bool required : td.weapon_required)
			ds << static_cast<quint8>(required ? 1 : 0);
	}

	return data;
}

void TeamLoadoutDialogModel::restoreState(const QByteArray& state)
{
	QDataStream ds(state);

	qint32 num_teams;
	ds >> num_teams;

	for (int t = 0; t < static_cast<int>(num_teams) && t < MAX_TVT_TEAMS; ++t) {
		team_data& td = Team_data[t];

		// Clear arrays before repopulating
		td.num_ship_choices = 0;
		for (int i = 0; i < MAX_SHIP_CLASSES; ++i) {
			td.ship_list[i]             = -1;
			td.ship_list_variables[i][0] = '\0';
			td.ship_count[i]            = 0;
			td.ship_count_variables[i][0] = '\0';
		}

		qint32 num_ship_choices;
		ds >> num_ship_choices;
		td.num_ship_choices = static_cast<int>(num_ship_choices);

		for (int i = 0; i < static_cast<int>(num_ship_choices); ++i) {
			qint32 ship_id, ship_count;
			QString ship_var, count_var;
			ds >> ship_id >> ship_var >> ship_count >> count_var;

			td.ship_list[i]  = static_cast<int>(ship_id);
			strncpy(td.ship_list_variables[i], ship_var.toLatin1().constData(), TOKEN_LENGTH - 1);
			td.ship_list_variables[i][TOKEN_LENGTH - 1] = '\0';
			td.ship_count[i] = static_cast<int>(ship_count);
			strncpy(td.ship_count_variables[i], count_var.toLatin1().constData(), TOKEN_LENGTH - 1);
			td.ship_count_variables[i][TOKEN_LENGTH - 1] = '\0';
		}

		// Clear weapon arrays before repopulating
		td.num_weapon_choices = 0;
		for (int i = 0; i < MAX_WEAPON_TYPES; ++i) {
			td.weaponry_pool[i]               = -1;
			td.weaponry_pool_variable[i][0]   = '\0';
			td.weaponry_count[i]              = 0;
			td.weaponry_amount_variable[i][0] = '\0';
			td.weapon_required[i]             = false;
		}

		qint32 num_weapon_choices;
		quint8 do_not_validate;
		ds >> num_weapon_choices >> do_not_validate;
		td.num_weapon_choices = static_cast<int>(num_weapon_choices);
		td.do_not_validate    = (do_not_validate != 0);

		for (int i = 0; i < static_cast<int>(num_weapon_choices); ++i) {
			qint32 pool_id, wpn_count;
			QString pool_var, amount_var;
			ds >> pool_id >> pool_var >> wpn_count >> amount_var;

			td.weaponry_pool[i] = static_cast<int>(pool_id);
			strncpy(td.weaponry_pool_variable[i], pool_var.toLatin1().constData(), TOKEN_LENGTH - 1);
			td.weaponry_pool_variable[i][TOKEN_LENGTH - 1] = '\0';
			td.weaponry_count[i] = static_cast<int>(wpn_count);
			strncpy(td.weaponry_amount_variable[i], amount_var.toLatin1().constData(), TOKEN_LENGTH - 1);
			td.weaponry_amount_variable[i][TOKEN_LENGTH - 1] = '\0';
		}

		qint32 req_count;
		ds >> req_count;
		for (int i = 0; i < static_cast<int>(req_count) && i < MAX_WEAPON_TYPES; ++i) {
			quint8 req;
			ds >> req;
			td.weapon_required[i] = (req != 0);
		}
	}
}

} // namespace fso::fred::dialogs
