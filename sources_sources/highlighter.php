<?php
/*
* Copyright (c) 2010 Christopher W. Allen-Poole
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
//FIXME: This should automatically prepend the MIT license unless another
//	 license is noted on the file.

$req = $_SERVER[ 'REQUEST_URI' ];
$req = str_replace( '%20', ' ', $req );
//FIXME: This needs to be split into multiple files.  I had thought it could
//	 be avoided, but it just does not seem tenable anymore. 
// issue with htaccess.  In some circumstances, this isn't properly reading $_GET.
if( strpos( $req, "download=" ) !== FALSE )
{
    $contents = get_file_contents( $req );
    $name     = get_file_name( $req );
    header( 'Content-type: text/plain' );
    header( 'Content-Disposition: attachment; filename="' . $name .'"' );
    print( $contents );
    exit;
}

require_once( "geshi.php" );

function get_file_name( $req )
{
    if( ( $pos = strpos( $req, "?" ) ) !== FALSE ) $req = substr( $req, 0, $pos );
    return ( $req );
}
function get_file_contents( $req )
{
    return trim( file_get_contents( $_SERVER[ 'DOCUMENT_ROOT' ] . DIRECTORY_SEPARATOR . get_file_name( $req ) ) );
}

function get_extension( $file )
{
    $path_info = pathinfo( $file );
    return @$path_info['extension'];
}
$ext = get_extension($req);
// all of these are conditions where the highlighter should simply
// output the file content and then exit.
if( is_null( $ext ) || !$ext || in_array( $ext, array( "txt") ) )
{
    header( 'Content-type: text/plain' );
    echo get_file_contents($req);
    exit;
}
function get_language_from_extension( $ext )
{
    $ret = "";
    switch( strtolower( $ext ) )
    {
        case 'as':
            //default to VM1
            $ret = 'actionscript';
            break;
        case 'sh':
            $ret = 'bash';
            break;
        case 'htaccess':
        case 'htgroup':
        case 'htpasswd':
        case 'conf': # This may be switched??
            $ret = 'apache';
            break;
        case 'c':
        case 'h':
            $ret = 'c';
            break;
        case 'cpp':
        case 'hh':
        case 'hpp':
        case 'hxx':
        case 'h++':
        case 'cc':
        case 'cpp':
        case 'cxx':
        case 'c++':
            $ret = 'CPP';
            break;
        case 'css':
            $ret = 'CSS';
            break;
        case 'bat':
            $ret = 'dos';
            break;
        case 'html':
        case 'xhtml':
            $ret = 'html4strict';
            break;
        case 'java':
            $ret = 'java5';
            break;
        case 'js': // defaulting this to jQuery... this will default to more highlighting.
            $ret = 'jquery';
            break;
        case 'mxml':
            $ret = 'mxml';
            break;
        case 'pl':
        case 'pm':
            $ret = 'perl6';
            break;
        case 'py':
        case 'pyw':
        case 'pyc':
        case 'pyo':
        case 'pyd':
            $ret = 'python';
            break;
        case 'sql':
            $ret = 'sql';
            break;
        case 'php':
        case 'phtml':
        case 'php3':
        case 'php4':
        case 'php5':
        case 'phps':
            $ret = 'php';
            break;
        case 'xml':
            $ret = 'xml';
            break;
        default:
            if( file_exists( dirname( __FILE__ ) . "geshi/$ext.php" ) )
            {
                $ret = $ext;
            }
            else
            {
                throw new Exception( 'Language cannot be recognized from extension.  Please set `language` GET or POST variable' );
            }
            break;
            
    }
    return $ret;
}


?>
<html><head><title><?php echo $req; ?></title></head><body>
You're looking at <?php echo $req; ?>.<br /><a href="<?php echo $_SERVER[ 'REQUEST_URI' ];?>?download=1">Click here to download it as a text file.</a>
<code><?php
$file = get_file_contents( $req );
$lang = ( isset( $_REQUEST[ 'language' ] ) )? 
            $_REQUEST[ 'language' ]: 
            get_language_from_extension( $ext );
$gesh = new GeSHi( $file, strtolower( $lang ) );
$gesh->enable_line_numbers( GESHI_FANCY_LINE_NUMBERS );
$gesh->line_style1 = 'font-weight: bold; vertical-align:top;';
$gesh->line_style2 = 'font-weight: bold; vertical-align:top; background-color: #EAEAEA';
//$gesh->enable_classes();
echo $gesh->parse_code();
?></code></body></html>
