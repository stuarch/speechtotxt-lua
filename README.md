# speechtotxt-lua
Library for using speech to text in Lua offline. Using PocketSphinx and SphinxBase.

### Install

Please install [pocketsphinx](https://github.com/cmusphinx/pocketsphinx) and [sphinxbase](https://github.com/cmusphinx/sphinxbase) first.

Then, install [Lua5.1](https://www.lua.org/). The following commands is for Ubuntu and Debian User.
```
$ sudo apt-get install lua5.1 liblua5.1-dev
```
Clone, compile, and then install this.
```
$ git clone https://github.com/stuarch/speechtotxt-lua.git
$ cd speechtotxt-lua
$ make
$ sudo make install
```

### Usage

```
speechtotxt = require("speechtotxt")

--first argument is for -hmm, second is for -lm, the last is for --dict.
--left it nil for default.
speech = speechtotxt.init(nil,nil,nil)

--get the speechtotxt result
--Address of sound file as argument
txt = speech:open("./test.wav")
print(txt)

--close context
speech:close()
```
