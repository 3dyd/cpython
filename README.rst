This is Python version 3.12.11 patched for Windows Vista 32-bit
================================================================

``python-3.x.x-embed-win32-VISTA.zip`` is a Windows embeddable package (32-bit), the same as you can find in standard Python downloads.

``python-3.x.x-pip+venv-win32-VISTA.zip`` is a Python package (32-bit) with pip and venv modules.

How to Build
============

Requires Microsoft Visual Studio (Community version with C++ workload should be enough) and Python installed.

.. code-block:: bash

   git clone --branch 3.12.11-vista --single-branch https://github.com/3dyd/cpython.git
   cd cpython
   PCbuild\build.bat -p Win32
   py ./PC/layout/main.py -s . --arch win32 -b ./PCbuild/win32 --copy ./build --preset-embed

The last command will create layout for ``*-embed-*`` package in the ``./build`` directory.

For ``*-pip+venv-*`` package is used this command:

.. code-block:: bash

   py ./PC/layout/main.py -s . --arch win32 -b ./PCbuild/win32 --copy ./build --flat-dlls --include-venv --include-pip
