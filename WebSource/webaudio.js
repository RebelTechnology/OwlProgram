/*
 * http://thealphanerd.io/blog/from-faust-to-webaudio/
 */

var owl = owl || {};

// This should be made to only make a new context if one does not exist

if (!owl.context)
{
    owl.context = new AudioContext();
}

var WEB_setup = Module.cwrap('WEB_setup', 'number', ['number', 'number']);
var WEB_processBlock = Module.cwrap('WEB_processBlock', 'number', ['number', 'number']);
var WEB_setParameter = Module.cwrap('WEB_setParameter', 'number', ['number', 'number']);
var WEB_getPatchName = Module.cwrap('WEB_getPatchName', 'string', []);
var WEB_getParameterName = Module.cwrap('WEB_getParameterName', 'string', ['number']);
var WEB_getMessage = Module.cwrap('WEB_getMessage', 'string', []);
var WEB_getStatus = Module.cwrap('WEB_getStatus', 'string', []);

owl.dsp = function () {
    var that = {};
    that.model = {
	playing: false
    };
    that.vectorsize = 2048;      
    console.log("setup[fs "+owl.context.sampleRate+"][bs "+that.vectorsize+"]");
    WEB_setup(owl.context.sampleRate, that.vectorsize);
    for (i = 0; i < 5; i++)
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
	var i, j;
	for (i = 0; i < that.numIn; i++)
	{
            var input = e.inputBuffer.getChannelData(i);
            var dspInput = HEAPF32.subarray(dspInChans[i] >> 2, (dspInChans[i] + that.vectorsize * that.ptrsize) >> 2);

            for (j = 0; j < input.length; j++) {
		dspInput[j] = input[j];
            }
	}

	WEB_processBlock(that.ins, that.outs);

	for(i = 0; i < that.numOut; i++){
            var output = e.outputBuffer.getChannelData(i);
            var dspOutput = HEAPF32.subarray(dspOutChans[i] >> 2, (dspOutChans[i] + that.vectorsize * that.ptrsize) >> 2);
            for (j = 0; j < output.length; j++) {
		output[j] = dspOutput[j];
            }
	}
	return that;
    };

    // Connect to another node
    that.connect = function (node) {
	if(node.scriptProcessor){
            that.scriptProcessor.connect(node.scriptProcessor);
	}else{
            that.scriptProcessor.connect(node);
	}
	return that;
    };

    // Bind to Web Audio
    that.getParameterName = function(pid){
	return WEB_getParameterName(pid);
    }

    that.getPatchName = function(){
	return WEB_getPatchName();
    }

    that.play = function () {
	that.scriptProcessor.connect(owl.context.destination);
	that.model.playing = true;
	return that;
    };

    that.pause = function () {
	that.scriptProcessor.disconnect(owl.context.destination);
	that.model.playing = false;
	return that;
    };

    that.toggle = function() {
	if (that.model.playing) {
            that.pause()
	}
	else {
            that.play();
	}
	return that;
    }

    that.update = function (key, val) {
	WEB_setParameter(key, val);
	console.log("set parameter "+key+": "+val);
	return that;
    };

    that.init = function () {
	var i;
	that.ptrsize = 4; // assuming pointer in emscripten are 32bits
	// that.vectorsize = 2048;
	that.samplesize = 4;

	// Get input / output counts
	that.numIn = that.getNumInputs();
	that.numOut = that.getNumOutputs();

	// Setup web audio context
	that.scriptProcessor = owl.context.createScriptProcessor(that.vectorsize, that.numIn, that.numOut);
	that.scriptProcessor.onaudioprocess = that.compute;

	// TODO the below calls to malloc are not yet being freed, potential memory leak
	// allocate memory for input / output arrays
	that.ins = Module._malloc(that.ptrsize * that.numIn);

	// assign to our array of pointer elements an array of 32bit floats, one for each channel. currently we assume pointers are 32bits
	for (i = 0; i < that.numIn; i++) { 
            // assign memory at that.ins[i] to a new ptr value. maybe there's an easier way, but this is clearer to me than any typedarray magic beyond the presumably TypedArray HEAP32
            HEAP32[(that.ins >> 2) + i] = Module._malloc(that.vectorsize * that.samplesize); 
	}

	//ptrsize, change to eight or use Runtime.QUANTUM? or what?
	that.outs = Module._malloc(that.ptrsize * that.numOut); 

	// assign to our array of pointer elements an array of 64bit floats, one for each channel. currently we assume pointers are 32bits
	for (i = 0; i < that.numOut; i++) { 
            // assign memory at that.ins[i] to a new ptr value. maybe there's an easier way, but this is clearer to me than any typedarray magic beyond the presumably TypedArray HEAP32
            HEAP32[(that.outs >> 2) + i] = Module._malloc(that.vectorsize * that.samplesize);
	}
	return that;
    };

    that.init();

    return that;
};
