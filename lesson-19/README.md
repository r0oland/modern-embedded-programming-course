Update for Code Composer Studio 11:
-----------------------------------
Code Composer Studio (CCS) 11 no longer comes with the GNU-ARM compiler.
(Before that, CCS came bundled with the GNU-ARM Linaro toolchain).

But CCS 11 can be easily configured to use other GNU-ARM toolchains, for
example the GNU-ARM toolchain available from ARM-Developer website:

https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads


Changed CCS Project
-------------------
The CCS project in this lesson has been updated to use the GNU v10.3.1
from the directory C:/qp/qtools/gnu_arm-none.eabi. This latter directory
is the default location of installing the "QP-bundle", which contains
the GNU-ARM toolchain.


Adding an External GNU-ARM Compielr to CCS
------------------------------------------
However, you can also install the GNU-ARM compiler separately in any
other directory. Then, you need to update the "Tool discovery path" 
in the Preferences dialog box.

To open the Preferences dialog box, first select the "Project|Properties"
menu and go to the Gneral section (on the left). In the "Compiler version"
selection, click on the "More..." button.


Quantum Leaps,
2021-11-22
