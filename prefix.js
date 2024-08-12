
var Module = Module || {};

Module.print = (...args) => {
    const text = args.join(" ");
    console.log(text);
};

Module.canvas = document.getElementById("canvas");

setTimeout(() => {
    Module.canvas.style.width = Math.floor(Module.canvas.width / window.devicePixelRatio) + 'px';
    Module.canvas.style.height = Math.floor(Module.canvas.height / window.devicePixelRatio) + 'px';
}, 300);

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


Module.initialGameState = null;

Module.call_getInitialGameState = () => {
    fetch("/api/init", { method: 'GET' })
    .then(async (response) => {
        const json = await response.json();
        Module.initialGameState = json;
    });
}
