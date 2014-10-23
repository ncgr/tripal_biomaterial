<?php

// expand the biomaterial.
$biomaterial = $variables['node']->biomaterial;
$biomaterial_id = $biomaterial->biomaterial_id;

//Display data only when a biomaterial has an associated stock
$stock_id = db_query('SELECT stock_id FROM chado.biomaterial WHERE biomaterial_id = :bg_id', array(':bg_id' => $biomaterial_id))->fetchField(); 

if (count($stock_id) > 0) { ?>
 <div class="tripal_biomaterial-data-block-desc tripal-data-block-desc">Stocks related to this biomaterial</div><?php
    
       // the $headers array is an array of fields to use as the colum headers.
       // additional documentation can be found here
       // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
     $headers = array('Name', 'Uniquename','Description');

      // the $rows array contains an array of rows where each row is an array
      // of values for each column of the table in that row.  Additional documentation
      // can be found here:
     // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
     $rows=array();
    
     //get stock_id specific data

     $values = array( 
    'stock_id' => $stock_id
     );
     $stock_value=chado_generate_var('stock', $values);

     $result = db_query('SELECT chado_stock.nid FROM {chado_stock} WHERE chado_stock.stock_id = :og_id', array(':og_id' => $stock_id))->fetchField();
     
     //linking name feild to nodes.

     $link = l($stock_value->name, 'node/' . $result ,array('attributes' => array('target' => '_blank')));
          $rows[]=array(
          $link,
          $stock_value->uniquename,
          $stock_value->description,
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
   
