#!/usr/bin/env bash
# Copyright (c) 2010 Christopher W. Allen-Poole
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

#FIXME: This should be dynamically referring to the most recent prod. release
major_version=1
minor_version=4
revis_version=8
#default to using the zip
from_vc=false

final_version=$major_version.$minor_version.$revis_version
project_name=
while getopts 'v:n:' OPTION
do
  case $OPTION in
  v)    
        #TODO: version control support
        final_version=$OPTARG
        ;;
  n)	project_name=$OPTARG
        ;;
  esac
done


if [[ -z "$minor_version" ]]; then
    if [[ $major_version =~ .*\..* ]]; then
        final_version=$major_version
    else
        final_version=$major_version.0.0
    fi
fi

if [[ $final_version =~ [0-9]+\.[0-9]+\.{0,1}[0-9]* ]]; then
    echo "Loading version" $final_version
else
    #FIXME: This should be dynamically referring to the most recent prod. release
    echo "\"$final_version\" is not formatted correctly.  Defaulting to $major_version.$minor_version.$revis_version";
    final_version=$major_version.$minor_version.$revis_version
fi

folder="symfony-${final_version}"
src="$folder.zip"
get="http://www.symfony-project.org/get/$src"
echo "Getting $get"
wget $get 
unzip "$src"
mkdir ./lib
mkdir ./lib/vendor
mv $folder ./lib/vendor/symfony
rm $src
php lib/vendor/symfony/data/bin/symfony -V

if [[ -n "$project_name" ]]; then
    php lib/vendor/symfony/data/bin/symfony generate:project $project_name
    chmod 777 symfony
fi
