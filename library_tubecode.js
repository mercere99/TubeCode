mergeInto(LibraryManager.library, {
    $tube_cb: { restart:0, step:0, play:0, end:0, play_step:0, is_paused:true },

    Tube_SetButtonCallbacks__deps: ['$tube_cb'],
    Tube_SetButtonCallbacks: function(restart_cb, step_cb, play_cb, end_cb, play_step_cb) {
        tube_cb.restart = restart_cb;
        tube_cb.step = step_cb;
        tube_cb.play = play_cb;
        tube_cb.end = end_cb;
        tube_cb.play_step = play_step_cb;
    }

});

