@echo off
::  ==========================================================================
::  Product: QP/C script for running MSquared Resource Standard Metrics (RSM)
::
:: Contact information:
:: https://state-machine.com
:: mailto:info@state-machine.com
::  ==========================================================================
setlocal

set RCMHOME="C:\tools\MSquared\M2 RSM"

set RSM_OUTPUT=rsm_metrics.txt
set RSM_INPUT=*.h *.c

%RCMHOME%\rsm.exe -fd -n -xNOCOMMAND -xNOCONFIG -u"File cfg rsm_qpc.cfg" %RSM_INPUT% > %RSM_OUTPUT%

endlocal
