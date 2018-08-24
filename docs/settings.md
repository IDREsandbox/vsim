
# how to add a setting

ex: other_settings, year, year enabled

- add to flatbuffers
- add static copySetting() to VSimRoot, makes VSimRoot copyable
- add SettingT to VSimRoot for simple usage, make_unique in constructor
- use copySetting() in VSimRoot::copy(), VSimSerializer createSettings and readSettings
- somewhere have gatherSettings() and extractSettings() connected to VSimApp::sReset and VSimApp::sAboutToSave
- use settingsLocked() if relevant