# AlphaZe**

AlphaZe** is an adaptation of [AlphaZero](https://arxiv.org/abs/1712.01815) and combines is with our own version of the so-called [Perfect Information Monte-Carlo](https://arxiv.org/abs/1106.0669) algorithm to deal with imperfect information games, such as DarkHex or Stratego. 

This project is build on the improved AlphaZero implementation of Johannes Czech, called [CrazyAra](https://github.com/QueensGambit/CrazyAra). It is also full integrated and part of the original CrazyAra reporsitory now. It is written in python as well as C++ and still maintained. For information about the backend, the model architecture we used in this work or installation steps, we would recommend to read the CrazyAra wiki. 

AlphaZe** is completely trained via reinforcement learning and do not us any form of supervised learning, this makes [RL instructions](https://github.com/BluemlJ/AlphaZe/blob/master/engine/src/rl/README.md) especially helpful.

## How to use AlphaZe**

First you have to download or build the binary to execute. An example binary can be found [here](https://drive.google.com/file/d/1rwoEFuBhXKdJXmp7M2NgnnzoCV_qgODD/view?usp=sharing)

We cannot guarantee that the binaries will run out of the box on every system. We provide a dockerfile to create an environment in which the binaries work.
We distinguish between binaries, with and without the ability to be trained using RL. To use a binary, it must always be used with a suitable model. Such models can be downloaded separately from the following [link](https://drive.google.com/file/d/1rwoEFuBhXKdJXmp7M2NgnnzoCV_qgODD/view?usp=sharing). For Hex and Darkhex we also provide a model that is not trained at all and can be used to test training. 

Copy the appropriate folder named model/... into the directory of the binary. Afterwards you can start the binary and use it with the [instructions of CrazyAra](https://github.com/QueensGambit/CrazyAra/wiki/Command-Line-Usage).
Instead of a correct FEN String (like in chess or CrazyAra), it is possible to use one of the FENfS Strings from the following examples to define a starting position for Stratego.

* aaaaaaaaKMaaaaaaaaEBaaaaaaaaaaDaaCLaaaaDaa__aa__aaaa__aa__aaaaaaOWaaNQaaaaaaaaaaaaaaaaaaXPaaaaaaaaPY r 0 
* MBaaaaaaaaCEaaaaaaaaKLaaaaaaaaaDaaDaaaaaaa__aa__aaaa__aa__aaOWaaXPaaQaaaaaaaaaaaPaaaaaaaaaNYaaaaaaaa r 0 
* MDaaaaaaaaCaaaaaaaaaaaaaaaaaaaKBaDLEaaaaaa__aa__aaaa__aa__aaaaaaaaaaaaaaaaaaaaWPaaaaaaaQXOaaaaaaaPYN r 0 

### Building your own binary

To build your own binary it is neccessary to install some additional libaries (which are also described in the dockerfile)
It is neccessary to change the game within the [CMakeList.txt](https://github.com/BluemlJ/AlphaZe/blob/master/engine/CMakeLists.txt). Stratego can be played, using MODE_STRATEGO, DarkHex and Hex via the MODE_OPEN_SPIEL parameter. Be careful, only one mode can be active at the same time. The binary is named after the active mode, i.e. StrategoAra is a binary able to play Stratego, OpenSpielAra, is able to play Hex. 

## How to train a model

To train a new model follow the [instructions](https://github.com/BluemlJ/AlphaZe/blob/master/engine/src/rl/README.md) described by CrazyAra. It is also necessary to activate USE_RL in the CMakeList and provide a initialized model, describing the architecture. 
