source set_global_env.sh

verifyresult=1

source common_func.h

verifysdk freetype      8d04c711e4ba7e551585b941a3e87fc1
verifysdk misc          3cbdbf797ac871c9aa77a9f848e1e748
verifysdk oglsdk        e9a9ed1044204a7813ecd1f9bf78d8d5

echo $verifyresult
