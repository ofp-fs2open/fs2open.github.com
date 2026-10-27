# Using OFP with Knossos.NET

This page shows how to switch a mod in [Knossos.NET](https://knossos.net) from the stock FSO engine to the Open Freespace Project (OFP) engine. It takes two steps: download the OFP build, then tell your mod to use it.

## 1. Download the OFP build

1. Open Knossos.NET.
2. Click the **Engine** tab in the menu on the left.
3. Click the **Custom Builds** tab in the menu along the top.
4. Find the latest **Open Freespace Project** build and click **Download**.

When the download finishes, OFP is installed alongside your other engine builds. Installing it doesn't change any of your mods.

## 2. Switch your mod to OFP

1. Click the **Develop** tab in the menu on the left.
2. Select your mod.
3. Open the **Packages** tab.
4. In the package's dependencies, find **FSO [ FSO ]** and replace it with **Open Freespace Project [ OFP ]**.
5. Click **Save Changes**.

That's it. Your mod now runs on OFP, and QtFRED opened for the mod is OFP's QtFRED.

## Good to know

- **Switching back:** repeat step 2 and replace **Open Freespace Project [ OFP ]** with **FSO [ FSO ]**.
- **Newer OFP builds:** when a new OFP build comes out, download it from **Custom Builds** the same way.
- **Missions made with OFP features:** a mission that uses an OFP-only feature needs OFP to play it. See [OFP Features](OFP-Features) for what OFP adds.
