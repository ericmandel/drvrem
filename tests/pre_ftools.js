var Module;
if( typeof Module !== "object" ){ Module = {}; }
Module["noExitRuntime"] = true;
Module["print"] = function(text) {
    if( arguments.length > 1 ){
	text = Array.prototype.slice.call(arguments).join(' ');
    }
    console.log(text);
    Module["outbuf"] += text + "\n";
};
Module["printErr"] = function(text) {
    if( arguments.length > 1 ){
	text = Array.prototype.slice.call(arguments).join(' ');
    }
    console.log(text);
    Module["errbuf"] += text + "\n";
};
