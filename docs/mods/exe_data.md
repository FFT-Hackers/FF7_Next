# EXE data

Sometimes texts and textures are stored inside the EXE, instead in the data directory.
In this case it is harder for modders to mod the game.

This feature allows modders to override data from the EXE via the
(Direct Mode)[direct_mode.md] feature.

Use the `save_exe_data` option to dump files to the direct/exe/ directory.
And then FFNx will look for those files directly instead of data from the EXE.

## Supported data

### FF8

- `battle_scans.msd`: Texts in battle scans. Note: this is not exactly the same
  format in the EXE, the msd format is used because it is a well documented format
  of FF8.
- `card_names.msd`: Card names. Note: this is not exactly the same
  format in the EXE, the msd format is used because it is a well documented format
  of FF8.
- `card_texts.msd`: Card module texts. Note: this is not exactly the same
  format in the EXE, the msd format is used because it is a well documented format
  of FF8.
- `draw_point.msd`: Draw point and Disc error messages
