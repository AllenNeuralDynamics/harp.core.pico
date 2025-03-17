# harp.device.pico-template

An RP2040-based Harp hardware/firmware project template.

## Features
* predefined *.gitignore* file to remove extraneous KiCAD (hardware) files.
### Hardware
* Schematic template for a generic Harp Device including:
  * ground-isolated Full Speed USB communication
  * 3.5mm audio jack for Harp-protocol time synchronization
  * RP2040 MCU and required "jellybean" components including:
    * 2MB external flash (Winbond MPN: W25Q16JVUXIQ TR).
  * 2 power supply chain options for power derived from either USB directly or from a DC barrel jack.
* Basic PCBA Layout with size hints for various existing enclosures
  * DC Barrel Jack (2.1 x 5.5mm, positive center)
  * USBC
  * 3.5mm audio jack for Harp synchronization signal
  * Ground lug
### Firmware
* basic "hello-world" project with corresponding file structure
* populated CMakeLists.txt for compliation
* placeholder USB descriptors for Manufacturer and Description fields (in the CMakeLists.txt)
* compilation [instructions](./firmware/README.md)
* harp protocol core library included as a submodule [harp.core.pico](https://github.com/AllenNeuralDynamics/harp.core.pico)

# Using this template
Start by clicking the "Use this Template" button in the upper right corner, or click [here](https://github.com/new?template_name=harp.device.pico-template&template_owner=AllenNeuralDynamics).
## Repository Setup
* If the project has a corresponding Allen Institute SIPE project number, prepend the repository description with: `{SIPE Project Number}:`.
* add the tag `Harp`
* Long-term: remove these instructions!

## Hardware
* Remove all unused hardware folder starter projects.
* 🔧 Design your hardware.
* In the PCBA silkscreen, add:
  * SIPE part number with full hardware semantic version appended in the format: `{SIPE Project Number}-E-{hw major}-{hw minor}-{hw patch}`. Version number fields range from 1-999 and include leading zeros.
  * [QR code](https://www.the-qrcode-generator.com/) linking to the Github repository.

## Firmware
* Update the *Manufacturer* and *Description* USB descriptors in the CMakeLists.txt
* 📝 Write your firmware.

# Github Release Conventions
Releases specify any vetted changes to the hardware or firmware.

Each release specifies:
* semantic versions of changed elements: hardware or firmware.
* SIPE part number (or part numbers if multiple are compatible)

Each hardware release includes:
* STEP file of the PCB (or link to corresponding CAD files) named with the SIPE part number
* schematic (PDF)
* Gerber files (with logos removed) of the PCBA
* Position files for component placement
* Bill-of-Materials for the PCBA

Each hardware release specifies:
* compatible SIPE part number(s)

> [!NOTE]
> SIPE part numbers only encode hardware semantic version major number, so multiple minor/patch hardware releases may point to the same SIPE part number.

> [!NOTE]
> It is possible for a project to have PCBA variants--possibly with very different functionality. In this case, reserve a hardware major version range (>100 suggested) for this variant and treat it as a distinct hardware release (i.e: with all corresponding included hardware files).


Each firmware release specifies:
* firmware semantic version
* compatible hardware (SIPE part number or part number range).

Each firmware release includes:
* compiled binary file (**\*.uf2**) of the firmware.

## Release Title
The release title is specified as follows:

`hw{hw major}.{hw minor}.{hw patch}-fw{fw major}.{fw minor}.{fw patch}`

If the release has firmware-only changes, you can drop the `hw*` section, and vice versa.

> [!NOTE]
> After Oct 2024, hardware major fields on all future releases will be equivalent to the major version number of the SIPE part number. See the [SIPE PCBA part numbering standard](https://alleninstitute.sharepoint.com/:w:/s/Instrumentation/EYsRN8q4jHJDmG5DNf-gaM0Bq418YMXollFxtB9d_NZ6pg?e=joLAvU) for more details.

The above field spec enables automation utilities to find the latest firmware version and automatically update compatible hardware. Since releases can include changes to some, but not other fields, unchanged fields are omitted. (i.e: firmware-only changes would be titled `fw{fw major}.{fw minor}.{fw patch}`.)

## Creating a New Release
1. Make a [git tag](https://git-scm.com/book/en/v2/Git-Basics-Tagging) based on the commit in the main branch that you'd like to create the release from. Use the naming convention specified in the [Release Title](#release-title) section.
2. Push the tag to Github.
3. From the repository's corresponding *Release* page, create a new release from the tag you just added. Give it the same title as the tag.
4. If the release includes new _firmware_, compile it locally, and upload a copy of the *.uf2* file as an attachment.
5. If the release includes new _hardware_, upload:
    1. the gerber files (zipped)
    2. the position files
    3. any manufacturing notes (component orientations)
    4. the bill-of-materials
    5. a PDF of the schematic.
