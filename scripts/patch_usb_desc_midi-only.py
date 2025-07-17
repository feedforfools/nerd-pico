import os
import shutil

Import("env")

# =================================================================
# CUSTOM DEVICE CONFIGURATION
# =================================================================
# Free VID/PID for open-source projects here: http://pid.codes/
#
# The default Teensy VID is 0x16C0.
# The default Teensy MIDI PID is 0x0485.

CUSTOM_VID = "0x4452"
CUSTOM_PID = "0x03C0"
MANUFACTURER_NAME = "Feedforfools"
PRODUCT_NAME = "Nerd Pico"
# =================================================================

# --- Helper function to create C-style char arrays ---
def to_c_char_array(s):
    return "{" + ",".join([f"'{c}'" for c in s]) + "}"

# --- Paths and Logic ---
platform = env.PioPlatform()
framework_dir = platform.get_package_dir("framework-arduinoteensy")
core_dir = os.path.join(framework_dir, "cores", "teensy4")
usb_desc_file = os.path.join(core_dir, "usb_desc.h")
usb_desc_backup_file = os.path.join(core_dir, "usb_desc.h.bak")

print("--- Custom USB Patcher (Final Version) ---")

custom_usb_definition = f"""
//**************************************************************************
//  CUSTOM MIDI-ONLY (added by patch script)
//**************************************************************************
#if defined(USB_MIDI_ONLY)
  #define VENDOR_ID             {CUSTOM_VID}
  #define PRODUCT_ID            {CUSTOM_PID}
  #define MANUFACTURER_NAME     {to_c_char_array(MANUFACTURER_NAME)}
  #define MANUFACTURER_NAME_LEN {len(MANUFACTURER_NAME)}
  #define PRODUCT_NAME          {to_c_char_array(PRODUCT_NAME)}
  #define PRODUCT_NAME_LEN      {len(PRODUCT_NAME)}
  #define EP0_SIZE		64
  #define NUM_ENDPOINTS         2
  #define NUM_USB_BUFFERS	14
  #define NUM_INTERFACES	1
  #define MIDI_INTERFACE        0
  #define MIDI_NUM_CABLES       1
  #define MIDI_TX_ENDPOINT      3
  #define MIDI_TX_SIZE          64
  #define MIDI_RX_ENDPOINT      4
  #define MIDI_RX_SIZE          64
  #define ENDPOINT3_CONFIG	ENDPOINT_TRANSMIT_BULK
  #define ENDPOINT4_CONFIG	ENDPOINT_RECEIVE_BULK
"""

string_to_find = "#if defined(USB_SERIAL)"
replacement_string = custom_usb_definition + "\n#elif defined(USB_SERIAL)"

with open(usb_desc_file, "r") as f:
    content = f.read()

if "#if defined(USB_MIDI_ONLY)" in content:
    print("Patch already applied. Nothing to do.")
else:
    print(f"Patching {usb_desc_file}...")
    if not os.path.exists(usb_desc_backup_file):
        print("Creating backup...")
        shutil.copyfile(usb_desc_file, usb_desc_backup_file)
    new_content = content.replace(string_to_find, replacement_string, 1)
    with open(usb_desc_file, "w") as f:
        f.write(new_content)
    print("Successfully patched usb_desc.h!")