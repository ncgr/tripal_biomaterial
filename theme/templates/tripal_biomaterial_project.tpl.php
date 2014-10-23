<?php

// expand the biomaterial to include the properties.
$biomaterial = $variables['node']->biomaterial;
$biomaterial = chado_expand_var($biomaterial,'table', 'project_biomaterial', array('return_array' => 1));
$biomaterialprojects = $biomaterial->project_biomaterial;
//print("<pre>".print_r($biomaterialprojects,true)."</pre>");
// print_r($biomaterialprojects);
if (count($biomaterialprojects) > 0) { ?>
 <div class="tripal_biomaterial-data-block-desc tripal-data-block-desc">Projects related to this biomaterial</div><?php
    
      // the $headers array is an array of fields to use as the colum headers.
     // additional documentation can be found here
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
     $headers = array('Name', 'Description');

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7

     $rows=array();

     foreach($biomaterialprojects as $project){
       $proj=$project->project_id;   
       $link = l($proj->name, 'project/' . $proj->project_id ,array('attributes' => array('target' => '_blank')));
        $rows[]=array(
        $link,
        $proj->description,
      );
    }
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
   
