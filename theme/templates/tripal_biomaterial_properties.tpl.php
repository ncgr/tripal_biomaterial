<?php
// expand the biomaterial to include the properties.
$biomaterial = $variables['node']->biomaterial;
$biomaterial = chado_expand_var($biomaterial,'table', 'biomaterialprop', array('return_array' => 1));
$biomaterialprops = $biomaterial->biomaterialprop;
$organism=$biomaterial->taxon_id->common_name;

 
// put the properties in an array so we can remove the biomaterial_description property
$properties = array();
if ($biomaterialprops) {
  foreach ($biomaterialprops as $property) {
    // we want to keep all properties but the biomaterial_description as that
    // property is shown on the base template page.
    if($property->type_id->name != 'Biomaterial Description') {
      $property = chado_expand_var($property,'field','biomaterialprop.value');
      $properties[] = $property;
    }
  }
}

if (count($properties) > 0) { ?>
  <div class="tripal_biomaterial-data-block-desc tripal-data-block-desc">Additional information about this biomaterial:</div><?php

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('Property Name', 'Value');

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();

  $bamURL=0;
  $bamfile;
  $biomaterial_id;
  foreach($properties as $property){
      GLOBAL $biomaterial_id;
     $biomaterial_id=$property->biomaterial_id->biomaterial_id; 
      if (preg_match("/bamURL/", $property->type_id->name)){
         GLOBAL $bamURL;
         GLOBAL $bamfile;
         ++$bamURL;
         $bamfile= $property->value;
       }
      else{
       }

      $rows[] = array(
      ucfirst(preg_replace('/_/', ' ', $property->type_id->name)),
      $property->value
    );
  }

  // add the properties as individual rows

  // the $table array contains the headers and rows array as well as other
  // options for controlling the display of the table.  Additional
  // documentation can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(
      'id' => 'tripal_biomaterial-table-properties',
      'class' => 'tripal-data-table'
    ),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );

  // once we have our table array structure defined, we call Drupal's theme_table()
  // function to generate the table.
  print theme_table($table);
  //print $biomaterial_id;
  if($bamURL==1){
      $reference=select_genome($organism);
      $file;
      $file=get_igv_template();//Partial template loaded
      $file.='<application-desc main-class="org.broad.igv.ui.Main">';
      $file.="<argument>$bamfile</argument>";
      $file.="<argument>-g</argument>";
      $file.="<argument>$reference</argument>";
      $file.="<argument>--preferences</argument>";
      $file.="<argument>http://www.broadinstitute.org/igv/projects/current/genomespace.properties</argument>";
      $file.="</application-desc>";
      $file.="</jnlp>";
      $path=path_to_theme();
      echo "
       <html>
         <body>
          <TABLE BORDER='0'>
            <TR>
             <TD width='10%'><form action='/$path/theme/templates/Launch_JNLP.php' method='post'>       
            <input type='hidden' name='file' value='$file'>
             <input type='image' src='/$path/images/logo.png'/>
             </form></TD>
              <TD><form>
               <a href=http://localhost:60151/load?file=$bamfile>
                  <img src='/$path/images/track.png' width='75' height='40'/>
              </a> 
           </form></TD>
          </TR>
         </TABLE>
       </body>
   </html>"; 
  }
}

//returns the partial template for the JNLP file launching IGV

function get_igv_template(){
      $xml_array=(
            '<?xml version="1.0" encoding="utf-8"?>
	     <jnlp
  	        spec="6.0+"
  		codebase="http://igv.broadinstitute.org/app/current">
  		<information>
    		  <title>IGV 2.3</title>
    		  <vendor>The Broad Institute</vendor>
 		  <homepage href="http://www.broadinstitute.org/igv"/>
    		  <description>IGV Software</description>
  		  <description kind="short">IGV</description>
  		  <icon href="IGV_64.gif"/>
  		  <icon kind="splash" href="IGV_64.gif"/>
  		  <offline-allowed/>
       		  <shortcut/>
 	       </information>
 	       <security>
     		 <all-permissions/>
  	       </security>
 	       <update check="background"/>
               <resources>
		<java version="1.6+" initial-heap-size="256m" max-heap-size="900m"/>    <jar href="igv.jar" download="eager" main="true" version="2.3.33"/>
    		<jar href="batik-codec.jar" download="eager" version="1.7"/>
    		<jar href="goby-io-igv.jar" download="lazy" version="1.0"/>
    		<property name="jnlp.versionEnabled" value="true"/>
    		<property name="java.net.preferIPv4Stack" value="true"/>
    		<property name="apple.laf.useScreenMenuBar" value="true"/>
    		<property name="com.apple.mrj.application.growbox.intrudes" value="false"/>
   		 <property name="com.apple.mrj.application.live-resize" value="true"/>
   		 <property name="com.apple.macos.smallTabs" value="true"/>
    		<property name="http.agent" value="IGV"/>
   		 <property name="production" value="true"/>
	  </resources>'
        ); 
     return $xml_array;
}


function select_genome($organism) {     
     $hash=array(
      'common bean' => 'http://data.comparative-legumes.org/genomes/phavu/Pvulgaris_218_v1.0.fa',
      'soybean' => 'http://data.comparative-legumes.org/genomes/glyma/Gmax_275_v2.0.fa',
     );
       if(key_exists($organism,$hash)){
          return $hash[$organism];
       }

}

