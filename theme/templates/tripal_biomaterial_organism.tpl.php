<?php

// expand the biomaterial to include the properties.
$biomaterial = $variables['node']->biomaterial;

$organism_id = $biomaterial->taxon_id->organism_id;

//print("<pre>".print_r($variables['node'],true)."</pre>");

if (count($organism_id) > 0) { ?>
   <div class="tripal_biomaterial-data-block-desc tripal-data-block-desc">Organism related to this biomaterial</div><?php
    $values = array(
    'organism_id' => $organism_id
  );

   $organism = chado_generate_var('organism', $values);
   // the $headers array is an array of fields to use as the colum headers.
   // additional documentation can be found here
   // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
   $headers = array('Species', 'Genus', 'Abbreviation');
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
   $rows=array();

   $result = db_query('SELECT chado_organism.nid FROM {chado_organism} WHERE chado_organism.organism_id = :og_id', array(':og_id' => $organism_id))->fetchField();
  
    $link = l($organism->species, 'node/' . $result ,array('attributes' => array('target' => '_blank')));
        $rows[]=array(
        $link,
        $organism->genus,
        $organism->abbreviation
      );
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
}
   
