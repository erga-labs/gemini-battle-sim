
var Module = Module || {};

Module.print = (...args) => {
    const text = args.join(" ");
    console.log(text);
};

Module.canvas = document.getElementById("canvas");

// used for stuff like telling the thing is being downloaded
// if error occurs, this is chaned
Module.setStatus = Module.print;

// error handling
Module.onerror = (event) => {
    alert("See console right now");
    Module.setStatus = (text) => {
        console.error("[post-exeception] " + text);
    };
};
