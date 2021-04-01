self.importScripts("fitsem.js");

self.onmessage = function(e) {
    let obj = e.data;
    let len = fitsem(obj.url, obj.mode, obj.start, obj.len);
    postMessage({len});
}
