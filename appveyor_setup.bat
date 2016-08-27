@echo off

call init_engine_res.bat

cd exec\tools\global_task

call set_global_env.bat

rem step01, download, extract, place the denpendencys

call extract_thirdparty.bat

echo All thing done...
