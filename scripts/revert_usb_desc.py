import os
import shutil

Import("env")

platform = env.PioPlatform()
framework_dir = platform.get_package_dir("framework-arduinoteensy")
core_dir = os.path.join(framework_dir, "cores", "teensy4")
usb_desc_file = os.path.join(core_dir, "usb_desc.h")
usb_desc_backup_file = os.path.join(core_dir, "usb_desc.h.bak")

print("--- Custom USB Reverter ---")

if os.path.exists(usb_desc_backup_file):
    print(f"Backup file found. Restoring {usb_desc_file}...")
    shutil.move(usb_desc_backup_file, usb_desc_file)
    print("Revert successful.")
else:
    print("No backup file found. Nothing to revert.")