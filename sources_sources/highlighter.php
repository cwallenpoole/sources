<?php
$req = $_SERVER[ 'REQUEST_URI' ];
// %20 allows the creation of " .htaccess" files.
// remove ".." to prevent access to things located elsewhere on the server!
$req = str_replace( array('%20','..'), array(' ', ''), $req );

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
    return $path_info['extension'];
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
            get_language_from_extension( get_extension( $req ) );
$gesh = new GeSHi( $file, strtolower( $lang ) );
$gesh->enable_line_numbers( GESHI_FANCY_LINE_NUMBERS );
$gesh->line_style1 = 'font-weight: bold; vertical-align:top;';
$gesh->line_style2 = 'font-weight: bold; vertical-align:top; background-color: #EAEAEA';
//$gesh->enable_classes();
echo $gesh->parse_code();
?></code></body></html>
