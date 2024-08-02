
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


Module.geminiResponse = null;

Module.call_getGeminiResponse = (secretKey) => {
    if (secretKey === "testSecretKey" && Module.geminiResponse !== null) {
        console.error("BUG: Calling gemini api route even if one is ongoing");
        return;
    }

    const postUrl = "/api/gemini";
    const headers = {
        "Authorization": "Bearer yoursecretkey",
    };
    const body = {
        "gameState": {
            "attackers": [
                [[1, 2], [3], [100]],
                [[4, 5], [2], [150]]
            ],
            "defenders": [
                [[6, 7], [1], [200]],
                [[8, 9], [3], [250]]
            ],
            "wall_health": [
                [[10], [11]],
                [[12], [13]]
            ],
            "last_moves": [
                ["player", "ai"]
            ],
            "world_size": [20, 20]
        }
    };

    fetch(postUrl, {
        method: 'POST',
        headers: headers,
        body: JSON.stringify(body),
    })
    .then(async (response) => {
        const json = await response.json();
        Module.geminiResponse = json;
    });
};
