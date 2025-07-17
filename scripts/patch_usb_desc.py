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
//  CUSTOM USB_MIDI (added by patch script)
//**************************************************************************
#elif defined(USB_MIDI)
  #define VENDOR_ID		{CUSTOM_VID}
  #define PRODUCT_ID		{CUSTOM_PID}
  #define MANUFACTURER_NAME	{to_c_char_array(MANUFACTURER_NAME)}
  #define MANUFACTURER_NAME_LEN	{len(MANUFACTURER_NAME)}
  #define PRODUCT_NAME		{to_c_char_array(PRODUCT_NAME)}
  #define PRODUCT_NAME_LEN	{len(PRODUCT_NAME)}
"""

with open(usb_desc_file, "r") as f:
    content = f.read()

if "CUSTOM USB_MIDI (added by patch script)" in content:
    print("Patch already applied. Nothing to do.")
else:
    print(f"Patching {usb_desc_file}...")
    if not os.path.exists(usb_desc_backup_file):
        print("Creating backup...")
        shutil.copyfile(usb_desc_file, usb_desc_backup_file)
    
    # Split content into lines
    lines = content.split('\n')
    
    # Find the line containing the start of the string to find
    start_line_index = None
    for i, line in enumerate(lines):
        if "#elif defined(USB_MIDI)" in line and "0x16C0" in lines[i+1] if i+1 < len(lines) else False:
            start_line_index = i
            break
    
    if start_line_index is not None:
        # Delete 7 lines starting from the found line
        del lines[start_line_index:start_line_index + 7]
        
        # Insert the custom USB definition at the same position
        custom_lines = custom_usb_definition.strip().split('\n')
        for j, custom_line in enumerate(custom_lines):
            lines.insert(start_line_index + j, custom_line)
        
        # Reconstruct the content
        new_content = '\n'.join(lines)
        
        with open(usb_desc_file, "w") as f:
            f.write(new_content)
        print("Successfully patched usb_desc.h!")
    else:
        print("Could not find the USB_MIDI section to replace!")