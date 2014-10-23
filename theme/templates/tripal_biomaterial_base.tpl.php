<?php
$biomaterial = $variables['node']->biomaterial;

// get the biomaterial description.  The first iteration of the biomaterial
// module incorrectly stored the biomaterial description in the Drupal 
// node->body field.  Also, the biomaterial.descriptin field is only 255
// characters which is not large neough. Therefore, we store the description
// in the  chado.biomaterialprop table.  For backwards compatibility, we 
// will check if the node->body is empty and if not we'll use that instead.
// If there is data in the biomaterial.description field then we will use that, but
// if there is data in the biomaterialprop table for a descrtion then that takes 
// precedence 
$description = '';
if (property_exists($node, 'body')) {
  $description = $node->body;
}
if ($biomaterial->description) {
  $description = $biomaterial->description;
}
else {
  $biomaterialprop = tripal_biomaterial_get_property($biomaterial->biomaterial_id, 'Biomaterial Description');
  $description = $biomaterialprop->value;
} ?>

<div class="tripal_biomaterial-data-block-desc tripal-data-block-desc"></div><?php

// the $headers array is an array of fields to use as the colum headers. 
// additional documentation can be found here 
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
// This table for the biomaterial has a vertical header (down the first column)
// so we do not provide headers here, but specify them in the $rows array below.
$headers = array();

// the $rows array contains an array of rows where each row is an array
// of values for each column of the table in that row.  Additional documentation
// can be found here:
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7 
$rows = array();
// Biomaterial Name row
$rows[] = array(
  array(
    'data' => 'Biomaterial Name',
    'header' => TRUE,
    'width' => '20%',
  ),
  $biomaterial->name
);
// allow site admins to see the feature ID
if (user_access('administer tripal')) {
  // Biomaterial ID
  $rows[] = array(
    array(
      'data' => 'Biomaterial ID',
      'header' => TRUE,
      'class' => 'tripal-site-admin-only-table-row',
    ),
    array(
      'data' => $biomaterial->biomaterial_id,
      'class' => 'tripal-site-admin-only-table-row',
    ),
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
    'id' => 'tripal_biomaterial-table-base',
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
if ($description) { ?>
  <div style="text-align: justify"><?php print $description; ?></div> <?php
}
