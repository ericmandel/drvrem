self.importScripts("ftools.js");

self.onmessage = function(e) {
    let obj = e.data;
    // C printf function is redefined to add to this buffer
    Module["outbuf"] = "";
    Module["errbuf"] = "";
    // call the specified routine and post the results back to the main thread
    switch(obj.cmd){
    case "listhead":
	listhead(obj.file);
	break;
    case "liststruc":
	liststruc(obj.file);
	break;
    case "imstat":
	imstat(obj.file);
	break;
    case "tablist":
	tablist(obj.file);
	break;
    default:
	Module["errbuf"] = `ERROR: unknown cmd: ${obj.cmd}`;
	break;
    }
    postMessage({cmd: obj.cmd,
		 out: Module["outbuf"], err: Module["errbuf"]});
}
