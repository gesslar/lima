# ed_start

### NAME

    ed_start() - start an ed session

### SYNOPSIS

    string ed_start(string file | void, int restricted | void)

### DESCRIPTION

    This efun is available only if __OLD_ED__ is not defined.

    The  internal editor is started, optionally loading 'file' for editing.
    The resulting output is returned.  The editor session  remains  active,
    and further calls to ed_cmd() can be used to send commands to it.

    If restricted is 1, the commands that change which file is being edited
    will be disabled.

    Only one ed session can be active per object at a time.

