/*
 * http://thealphanerd.io/blog/from-faust-to-webaudio/
 */

var owl = owl || {};

owl.audioContext = null;

owl.webAudioApiIsAvailable = function(){
    return (!!window.AudioContext || !!window.webkitAudioContext);
};

// Cross-browser compatibility for getUserMedia
navigator.getUserMedia = navigator.getUserMedia ||
    navigator.webkitGetUserMedia ||
    navigator.mozGetUserMedia ||
    navigator.msGetUserMedia;


owl.initPatchAudio = function () {
    if(owl.webAudioApiIsAvailable()){
        var AudioContext = window.AudioContext || window.webkitAudioContext;
        owl.audioContext = owl.audioContext || new AudioContext(); // makes sure there's only one audioContext 
    } else {
        throw new Error('no web audio api');
    }
    
    var WEB_setup = Module.cwrap('WEB_setup', 'number', ['number', 'number']);
    var WEB_processBlock = Module.cwrap('WEB_processBlock', 'number', ['number', 'number']);
    var WEB_setParameter = Module.cwrap('WEB_setParameter', 'number', ['number', 'number']);
    var WEB_getPatchName = Module.cwrap('WEB_getPatchName', 'string', []);
    var WEB_getParameterName = Module.cwrap('WEB_getParameterName', 'string', ['number']);
    var WEB_getMessage = Module.cwrap('WEB_getMessage', 'string', []);
    var WEB_getStatus = Module.cwrap('WEB_getStatus', 'string', []);
    var WEB_setButtons;
    var WEB_getButtons;
    var WEB_processMidi;
    try {
        WEB_setButtons = Module.cwrap('WEB_setButtons', 'number', ['number']);
        WEB_getButtons = Module.cwrap('WEB_getButtons', 'number', []);
	WEB_processMidi = Module.cwrap('WEB_processMidi', 'number', ['number', 'number', 'number', 'number']);
    }catch(x){}

    var that = {};
    that.model = {
	inputNode: null,
	fileNode: null, // owl.audioContext.createMediaElementSource(document.getElementById('patch-test-audio')),
	micNode: null
    };
    that.vectorsize = 2048;      
    console.log("audio[fs "+owl.audioContext.sampleRate+"][bs "+that.vectorsize+"]");
    WEB_setup(owl.audioContext.sampleRate, that.vectorsize);
    for(var i = 0; i < 5; i++)
	console.log("parameter "+i+": "+WEB_getParameterName(i));

    // Bind to C++ Member Functions
    that.getNumInputs = function () {
	return 2; // DSP_getNumInputs(that.ptr);
    };

    that.getNumOutputs = function () {
	return 2; //DSP_getNumOutputs(that.ptr);
    };

    that.getMessage = function() {
	return WEB_getMessage();
    }

    that.getStatus = function() {
	return WEB_getStatus();
    }

    that.compute = function (e) {
	var dspOutChans = HEAP32.subarray(that.outs >> 2, (that.outs + that.numOut * that.ptrsize) >> 2);
	var dspInChans = HEAP32.subarray(that.ins >> 2, (that.ins + that.ins * that.ptrsize) >> 2);
	for (var i = 0; i < that.numIn; i++)
	{
	    var input = e.inputBuffer.getChannelData(i);
	    var dspInput = HEAPF32.subarray(dspInChans[i] >> 2, (dspInChans[i] + that.vectorsize * that.ptrsize) >> 2);

	    for (var j = 0; j < input.length; j++) {
		dspInput[j] = input[j];
	    }
	}

	WEB_processBlock(that.ins, that.outs);

	for(var i = 0; i < that.numOut; i++){
	    var output = e.outputBuffer.getChannelData(i);
	    var dspOutput = HEAPF32.subarray(dspOutChans[i] >> 2, (dspOutChans[i] + that.vectorsize * that.ptrsize) >> 2);
	    for (var j = 0; j < output.length; j++) {
		output[j] = dspOutput[j];
	    }
	}
	return that;
    };

    // Change the input into the OWL patch node
    that.connectInput = function (node) {
        that.clearInput();
        node.connect(that.scriptNode);
        that.model.inputNode = node;
    }

    that.clearInput = function () {
        if (that.model.inputNode) {
            that.model.inputNode.disconnect(that.scriptNode);
        }
        that.model.inputNode = null;
    }

    that.useMicrophoneInput = function () {
	if (that.model.micNode) {
	    that.connectInput(that.model.micNode);
	} else {
	    navigator.getUserMedia.call(navigator, {audio: true}, function (stream) {
                that.model.micNode = owl.audioContext.createMediaStreamSource(stream);
		that.connectInput(that.model.micNode);
	    }, function (err) {
		console.error(err);
	    });
	}
    }

    that.useFileInput = function (audioNode) {
        that.model.fileNode = that.model.fileNode || owl.audioContext.createMediaElementSource(audioNode);
        that.connectInput(that.model.fileNode);
    }

    that.onFileSelect = function (files) {
	var fileUrl = files[0] ? URL.createObjectURL(files[0]) : '';
	var audioElement = document.getElementById('patch-test-audio');
	audioElement.src = fileUrl;
    }

    // Bind to Web Audio
    that.getParameterName = function(pid){
	return WEB_getParameterName(pid);
    }

    that.getPatchName = function(){
	return WEB_getPatchName();
    }

    that.update = function (key, val) {
	WEB_setParameter(key, val);
	return that;
    };

    that.setButtons = function(values) {
	if(WEB_setButtons){
            WEB_setButtons(values);
        }
        return that;
    };

    that.setPushButtonDown = function() {
        if(WEB_setButtons){
            var buttonState = WEB_getButtons();
            buttonState |= 0x02;
            buttonState &= ~0x04;
            buttonState |= 0x08; 
            WEB_setButtons(buttonState);
        }
    };

    that.setPushButtonUp = function() {
        if(WEB_setButtons){
            var buttonState = WEB_getButtons();
            buttonState &= ~0x02;
            buttonState |= 0x04;
            buttonState &= ~0x08; 
            WEB_setButtons(buttonState);
        }
    };

    that.getButtons = function() {
        if(WEB_getButtons){
            return WEB_getButtons();
        } else {
            return 0;
        }
    };

    that.toggleButton = function() {
        if(WEB_getButtons){
            var values = WEB_getButtons();
            values ^= 0x02; // PUSHBUTTON;
            values ^= 0x04; // GREEN_BUTTON;
            values ^= 0x08; // RED_BUTTON;
            WEB_setButtons(values);
        }
        return that;
    };

    that.getPushButtonLedColour = function(){
        if(WEB_getButtons){
            var buttonState = WEB_getButtons();
            if(buttonState & 0x04){ // bit 3 high is green
                return 'green';
            }
            if(buttonState & 0x08){ // bit 4 high is red
                return 'red';
            }
        }
        return null;
    };

    that.connectToOutput = function(options){
        options = options || {};
        if(options.outputs === 1){
            var splitter = owl.audioContext.createChannelSplitter(2);
            that.scriptNode.connect(splitter);
            that.outputNode = splitter;
        } else {
            that.outputNode = that.scriptNode;
        }
        that.outputNode.connect(owl.audioContext.destination);
    };

    that.disconnectFromOutput = function(){
        that.outputNode.disconnect();
    };


    that.processMidi = function(status, d1, d2) {
	var port = 0;
	if(WEB_processMidi){
    	    return WEB_processMidi(port, status, d1, d2);
	}else{
	    return 0;
	}
    }

    that.ptrsize = 4; // assuming pointer in emscripten are 32bits
    // that.vectorsize = 2048;
    that.samplesize = 4;

    // Get input / output counts
    that.numIn = that.getNumInputs();
    that.numOut = that.getNumOutputs();

    // Create OWL patch web audio node
    that.scriptNode = owl.audioContext.createScriptProcessor(that.vectorsize, that.numIn, that.numOut);
    that.scriptNode.onaudioprocess = that.compute;

    // TODO the below calls to malloc are not yet being freed, potential memory leak
    // allocate memory for input / output arrays
    that.ins = Module._malloc(that.ptrsize * that.numIn);

    // assign to our array of pointer elements an array of 32bit floats, one for each channel. currently we assume pointers are 32bits
    for (var i = 0; i < that.numIn; i++) { 
        // assign memory at that.ins[i] to a new ptr value. maybe there's an easier way, but this is clearer to me than any typedarray magic beyond the presumably TypedArray HEAP32
        HEAP32[(that.ins >> 2) + i] = Module._malloc(that.vectorsize * that.samplesize); 
    }

    //ptrsize, change to eight or use Runtime.QUANTUM? or what?
    that.outs = Module._malloc(that.ptrsize * that.numOut); 

    // assign to our array of pointer elements an array of 64bit floats, one for each channel. currently we assume pointers are 32bits
    for (var i = 0; i < that.numOut; i++) { 
        // assign memory at that.ins[i] to a new ptr value. maybe there's an easier way, but this is clearer to me than any typedarray magic beyond the presumably TypedArray HEAP32
        HEAP32[(that.outs >> 2) + i] = Module._malloc(that.vectorsize * that.samplesize);
    }

    return that;
};

export default owl;
