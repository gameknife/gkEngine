cd exec\tools\global_task

rem step01, download, extract, place the denpendencys

call extract_thirdparty.bat

rem step01-1, process default resource

call ..\resource_task\_obj2gmf_media.bat
call ..\resource_task\desktop_tga2dds_engine.bat

rem step02, download, extract, place the medias

call extract_media_pack.bat

rem step4, auto build, if possible



rem strp5, auto run, if possible

echo Press Any Key to Continue...

PAUSE