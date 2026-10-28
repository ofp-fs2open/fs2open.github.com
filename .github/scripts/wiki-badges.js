// Writes the version badges the wiki's table pages use (wiki/images/badges/*.svg).
// FSO colors match the version templates on the FreeSpace Wiki. Run with node after
// adding a version: node .github/scripts/wiki-badges.js

const fs = require('fs');
const path = require('path');

// [label, background, border, text]
const BADGES = {
	'fso-3.6.x':  ['FSO 3.6.x',  '#202035', '#505099', '#8888DD'],
	'fso-3.6.10': ['FSO 3.6.10', '#202035', '#505099', '#8888DD'],
	'fso-3.6.12': ['FSO 3.6.12', '#352020', '#995050', '#DD8888'],
	'fso-3.6.14': ['FSO 3.6.14', '#1d294b', '#597de3', '#648cff'],
	'fso-3.6.16': ['FSO 3.6.16', '#775500', '#eecc77', '#FFDD88'],
	'fso-3.7':    ['FSO 3.7',    '#203535', '#509999', '#88CCCC'],
	'fso-3.7.2':  ['FSO 3.7.2',  '#804413', '#CC5C00', '#FFDD88'],
	'fso-3.7.4':  ['FSO 3.7.4',  '#440085', '#8A00FF', '#c682ff'],
	'fso-3.8':    ['FSO 3.8',    '#652A71', '#D400FF', '#ED91FF'],
	'fso-19.0':   ['FSO 19.0',   '#156364', '#1d8788', '#31e5e7'],
	'fso-20.0':   ['FSO 20.0',   '#004a20', '#3dff41', '#abffc1'],   // no FreeSpace Wiki template; 20.4's colors, darker
	'fso-20.4':   ['FSO 20.4',   '#007003', '#3dff41', '#abffc1'],
	'fso-21.0':   ['FSO 21.0',   '#48238c', '#1d8788', '#ffcc23'],
	'fso-21.2':   ['FSO 21.2',   '#335533', '#1d8788', '#44ccff'],
	'fso-21.4':   ['FSO 21.4',   '#2e2e2e', '#CCCC33', '#CCCC33'],
	'fso-22.0':   ['FSO 22.0',   '#2e2e2e', '#44EE77', '#44EE77'],
	'fso-22.2':   ['FSO 22.2',   '#222222', '#6699FF', '#77AAFF'],
	'fso-23.0':   ['FSO 23.0',   '#103030', '#00ffff', '#ffaa11'],
	'fso-23.2':   ['FSO 23.2',   '#302410', '#ff9010', '#47e2d9'],
	'fso-23.4':   ['FSO 23.4',   '#2c2448', '#ff9010', '#ffc90e'],   // development version between 23.2 and 24.0 (used as a targeted version)
	'fso-24.0':   ['FSO 24.0',   '#2c2448', '#7364b0', '#ffc90e'],
	'fso-24.2':   ['FSO 24.2',   '#122410', '#779999', '#99aaaa'],
	'fso-25.0':   ['FSO 25.0',   '#051414', '#777799', '#9999aa'],
	'fso-26.0':   ['FSO 26.0',   '#222222', '#FF8866', '#FFAA66'],
	'fso-26.2':   ['FSO 26.2',   '#222222', '#EE22DD', '#CC22AA'],
};

// OFP badges get a triple border so they stand out from the FSO ones
const OFP_BADGES = {
	'ofp-26.1.0': ['OFP 26.1.0', '#0f2f4f', '#4fc3f7', '#e1f5fe'],
};

const CHAR_WIDTH = 7;   // Verdana bold at 11px, averaged; textLength makes the text fit exactly
const PAD = 8;
const HEIGHT = 20;
const OFP_HEIGHT = 30;

const out = path.join(__dirname, '..', '..', 'wiki', 'images', 'badges');
fs.mkdirSync(out, { recursive: true });

const text = (label, x, y, color, width) =>
	`<text x="${x}" y="${y}" fill="${color}" font-family="Verdana,DejaVu Sans,sans-serif" font-size="11" font-weight="bold" textLength="${width}" lengthAdjust="spacingAndGlyphs">${label}</text>`;

for (const [name, [label, bg, border, color]] of Object.entries(BADGES)) {
	const textWidth = label.length * CHAR_WIDTH;
	const width = textWidth + PAD * 2;
	const svg = `<svg xmlns="http://www.w3.org/2000/svg" width="${width}" height="${HEIGHT}" role="img" aria-label="${label}">
<title>${label}</title>
<rect x="0.5" y="0.5" width="${width - 1}" height="${HEIGHT - 1}" rx="3" fill="${bg}" stroke="${border}"/>
${text(label, PAD, 14, color, textWidth)}
</svg>
`;
	fs.writeFileSync(path.join(out, name + '.svg'), svg);
}

for (const [name, [label, bg, border, color]] of Object.entries(OFP_BADGES)) {
	const textWidth = label.length * CHAR_WIDTH;
	const width = textWidth + (PAD + 6) * 2;
	const svg = `<svg xmlns="http://www.w3.org/2000/svg" width="${width}" height="${OFP_HEIGHT}" role="img" aria-label="${label}">
<title>${label}</title>
<rect x="0.5" y="0.5" width="${width - 1}" height="${OFP_HEIGHT - 1}" rx="5" fill="${bg}" stroke="${border}"/>
<rect x="3.5" y="3.5" width="${width - 7}" height="${OFP_HEIGHT - 7}" rx="3" fill="none" stroke="${border}"/>
<rect x="6.5" y="6.5" width="${width - 13}" height="${OFP_HEIGHT - 13}" rx="2" fill="none" stroke="${border}"/>
${text(label, PAD + 6, 19, color, textWidth)}
</svg>
`;
	fs.writeFileSync(path.join(out, name + '.svg'), svg);
}
console.log(`Wrote ${Object.keys(BADGES).length + Object.keys(OFP_BADGES).length} badges to ${out}`);
