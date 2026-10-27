> **Revision information**
> OFP Revision: [#15](https://github.com/ofp-fs2open/fs2open.github.com/pull/15)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

The **props.tbl** defines prop classes: static, non-ship objects placed in a mission. Everything in it works in OFP as documented on the [FreeSpace Wiki](https://wiki.hard-light.net/index.php/Tables); this page covers only what OFP adds.

This table is one of the Modular Tables and can be extended with **xxx-prp.tbm**.

The OFP badge marks what OFP adds.

Code: [/prop/prop.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/prop/prop.cpp). See [OFP Tables](OFP-Tables) for every table.

## Contents

1. [Texture replacement](#texture-replacement)
   1. [$Texture Replace:](#texture-replace)

---

## Texture replacement

### $Texture Replace:

![OFP 26.1.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/ofp-26.1.0.svg)

- Replaces textures on the prop's model for every prop of this class. It works the same as `$Texture Replace:` in ships.tbl.
- Goes at the end of the prop entry, after `$Custom Strings` if the entry has that block.
- Followed by any number of `+old:` / `+new:` pairs. File extensions are ignored.
- Use `invisible` as the `+new:` texture to hide that part of the model.
- **Syntax:** `$Texture Replace:` followed by pairs of *String* values

#### +old:

- The name of the texture on the model to replace.
- **Syntax:** *String*

#### +new:

- The texture to use instead, or `invisible`.
- **Syntax:** *String*

**Sample**

```
$Name: Cargo Pod
$POF file: cargopod.pof
$Texture Replace:
+old: pod_hull
+new: pod_hull_red
+old: pod_decal
+new: invisible
```
