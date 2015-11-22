
A Polyphonic sample player with adjustable pitch & volume.
(c) GPL3 stc@binaura.net, 2012

The system creates multiple instances of sample players on creation of the [createPoly] object (name of sound file is set by its first argument, whereas number of parallell sounds are based on its second argument). There is an optional 3rd argument for setting the samplerate. This can be useful when working with tablets or phones where lower sampling rates are recommended considered to performance issues. 

Sample playback is very simple:

The samples can be triggered by sending a list on the left inlet of the object. First element is the pitch, second element is the volume of the corresponding sample. The overall volume of the sampler (including all the samples inside) can be controlled via its second inlet. 

Beware: too much objects can take too much performance especially on mobile devices & tablets. Built with Pd-Vanilla (should work in embedded projects, such as libPd, webPd, RJDJ and the like)