sh ../global_task/set_global_env.sh

mkdir $GKENGINE_HOME/../exec/bin32

copy %GKENGINE_HOME%\..\code\thirdparty\dxsdk\bin\* %GKENGINE_HOME%\..\exec\bin32
copy %GKENGINE_HOME%\..\code\thirdparty\oglsdk\bin\* %GKENGINE_HOME%\..\exec\bin32
copy %GKENGINE_HOME%\..\code\thirdparty\mscrt\* %GKENGINE_HOME%\..\exec\bin32