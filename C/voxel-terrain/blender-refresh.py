# auto_reload_obj.py
import bpy
import os
import traceback

OBJ_PATH = "/Users/Max/Documents/repositories.nosync/personal-development/c-quickies/C/voxel-terrain/test.obj"
RELOAD_INTERVAL = 1.5  # seconds

if "_auto_reload_state" not in globals():
    _auto_reload_state = {"last_mtime": None, "handler_registered": False}

def clear_scene():
    """Remove all objects safely, keeping at least one valid collection."""
    # Deselect everything
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.object.delete(use_global=False)

    # Purge orphan data to prevent leaks
    for data_block in (bpy.data.meshes, bpy.data.materials, bpy.data.images):
        for datablock in list(data_block):
            if datablock.users == 0:
                data_block.remove(datablock)

    # Ensure there is an active collection and scene
    if not bpy.context.scene.collection:
        bpy.context.scene.collection = bpy.data.collections.new("Collection")
        bpy.context.scene.collection.name = "Collection"

def import_obj():
    """Import the OBJ file safely into the active scene (Blender 4.x API)."""
    try:
        # For Blender 4.0+ the operator is wm.obj_import
        result = bpy.ops.wm.obj_import(filepath=OBJ_PATH)
        if 'FINISHED' in result:
            print("auto_reload_obj: re-imported OBJ successfully.")
        else:
            print("auto_reload_obj: OBJ import operator returned:", result)
    except AttributeError:
        # Fallback for Blender < 4.0
        try:
            result = bpy.ops.import_scene.obj(filepath=OBJ_PATH, axis_forward='-Z', axis_up='Y')
            if 'FINISHED' in result:
                print("auto_reload_obj: re-imported OBJ successfully (legacy operator).")
            else:
                print("auto_reload_obj: OBJ import operator returned:", result)
        except Exception:
            print("auto_reload_obj: error during OBJ import (both methods failed).")
            traceback.print_exc()
    except Exception:
        print("auto_reload_obj: error during OBJ import.")
        traceback.print_exc()

def reload_if_changed():
    """Timer callback: watch for file changes and re-import."""
    try:
        mtime = os.path.getmtime(OBJ_PATH)
    except Exception:
        if _auto_reload_state.get("last_mtime") is None:
            print("auto_reload_obj: OBJ file not found:", OBJ_PATH)
        return RELOAD_INTERVAL

    # First import
    if _auto_reload_state["last_mtime"] is None:
        _auto_reload_state["last_mtime"] = mtime
        print("auto_reload_obj: initial import:", OBJ_PATH)
        import_obj()
        return RELOAD_INTERVAL

    # Changed
    if mtime != _auto_reload_state["last_mtime"]:
        _auto_reload_state["last_mtime"] = mtime
        print("auto_reload_obj: change detected — reloading:", OBJ_PATH)
        clear_scene()
        import_obj()

    return RELOAD_INTERVAL

if not _auto_reload_state.get("handler_registered"):
    bpy.app.timers.register(reload_if_changed, first_interval=RELOAD_INTERVAL)
    _auto_reload_state["handler_registered"] = True
    print("auto_reload_obj: timer registered. Watching:", OBJ_PATH)
else:
    print("auto_reload_obj: already registered.")
