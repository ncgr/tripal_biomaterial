<?php
/**
 * @file
 * Implement the biomaterial node content type
 */

/**
 * Implementation of hook_node_info().
 *
 * This node_info, is a simple node that describes the functionallity of the module. It specifies
 * that the title(Biomaterial Name) and body(Description) set to true so that they information can be
 * entered
 *
 * @ingroup tripal_biomaterial
 */
function tripal_biomaterial_node_info() {
  return array(
    'chado_biomaterial' => array(
      'name'        => t('Biomaterial'),
      'base'        => 'chado_biomaterial',
      'description' => t('A biomaterial from the Chado database'),
      'has_title'   => TRUE,
      'locked'      => TRUE,
      'chado_node_api' => array(
        'base_table' => 'biomaterial',
        'hook_prefix' => 'chado_biomaterial',
        'record_type_title' => array(
          'singular' => t('Biomaterial'),
          'plural' => t('Biomaterials')
        ),
        'sync_filters' => array(
          'type_id' => FALSE,
          'organism_id' => FALSE
        ),
      ),
    ),
  );
}

/**
 * Implementation of hook_form().
 *
 *  This form takes the Biomaterial Title information and description from the user.
 *
 *  @parm $node
 *    The initialized node
 *
 *  @parm $form_state
 *    The state of the form, that has the user entered information that is neccessary for adding
 *    information to the biomaterial
 *
 *  @return $form
 *    An array as described by the Drupal Form API
 *
 *
 * @ingroup tripal_biomaterial
 */
function chado_biomaterial_form(&$node, $form_state) {
  $form = array();

  // Default values can come in the following ways:
  //
  // 1) as elements of the $node object.  This occurs when editing an existing biomaterial
  // 2) in the $form_state['values'] array which occurs on a failed validation or
  //    ajax callbacks from non submit form elements
  // 3) in the $form_state['input'[ array which occurs on ajax callbacks from submit
  //    form elements and the form is being rebuilt
  //
  // set form field defaults
  $biomaterial_id = null;
  $title = '';
  $description = '';

  // if we are editing an existing node then the biomaterial is already part of the node
  if (property_exists($node, 'biomaterial')) {
    $biomaterial = $node->biomaterial;
    // get the biomaterial default values.  When this module was first created
    // the biomaterial description was incorrectly stored in the $node->body field.
    // It is better to store it in the Chado tables.  However, the 'description'
    // field of the biomaterial table is only 255 characters.  So, we are going
    // to follow the same as the biomaterial module and store the description in
    // the biomaterialprop table and leave the biomaterial.description field blank.
    // however, for backwards compatibitily, we check to see if the description
    // is in the $node->body field. If it is we'll use that.  When the node is
    // edited the text will be moved out of the body and into the biomaterialprop
    // table where it should belong.
    if (property_exists($node, 'body')) {
      $description = $node->body;
    }
    else {
      $description = $biomaterial->description;
    }
    if (!$description) {
      $biomaterialprop = chado_get_property(
        array('table' => 'biomaterial', 'id' => $biomaterial->biomaterial_id),
        array('type_name' => 'Biomaterial Description', 'cv_name' =>'biomaterial_property')
      );
      $description = $biomaterialprop->value;
    }

    $title = $biomaterial->name;
    $biomaterial_id = $biomaterial->biomaterial_id;

    // keep track of the biomaterial id if we have.  If we do have one then
    // this is an update as opposed to an insert.
    $form['biomaterial_id'] = array(
      '#type' => 'value',
      '#value' => $biomaterial_id,
    );
  }

  // if we are re constructing the form from a failed validation or ajax callback
  // then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {
    $title       = $form_state['values']['title'];
    $description = $form_state['values']['description'];
  }
  // if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $title       = $form_state['input']['title'];
    $description = $form_state['input']['description'];
  }

  $form['title']= array(
    '#type'          => 'textfield',
    '#title'         => t('Biomaterial Title'),
    '#description'   => t('Please enter the title for this biomaterial. This appears at the top of the biomaterial page.'),
    '#required'      => TRUE,
    '#default_value' => $node->title,
  );

  $form['description']= array(
    '#type'          => 'textarea',
    '#title'         => t('Biomaterial Description'),
    '#description'   => t('A brief description of the biomaterial'),
    '#required'      => TRUE,
    '#default_value' => $description,
  );

  // Properties Form
  // ----------------------------------
  $select_options = array();
  $prop_cv = tripal_get_default_cv('biomaterialprop', 'type_id');
  $cv_id = $prop_cv ? $prop_cv->cv_id : NULL;
  if ($prop_cv = 'biomaterial_property') {
    // if this is the biomaterial_property CV then
    // we want to exclude the biomaterial description from being loaded as a stored property
    // because we want to use the property to replace the biomaterial.description field as it is
    // only 255 characters which isn't large enough. We don't want the user to set it
    // as a property even though it will be stored as a property.
    $cv_result = chado_select_record('cv',array('cv_id'),array('name' => 'biomaterial_property'));
    $cv_id = $cv_result[0]->cv_id;
    $select_options = tripal_get_cvterm_select_options($cv_id);
    $descrip_id = array_search('Biomaterial Description', $select_options);
    unset($select_options[$descrip_id]);
  }

  $instructions = t('To add properties to the drop down list, you must ' . l("add terms to the biomaterial_property vocabulary", "admin/tripal/chado/tripal_cv/cvterm/add") . ".");
  $details = array(
    'property_table' => 'biomaterialprop',
    'chado_id' => $biomaterial_id,
    'cv_id' => $cv_id,
    'additional_instructions' => $instructions,
    'select_options' => $select_options
  );
  chado_add_node_form_properties($form, $form_state, $details);

  // RELATIONSHIPS FORM
  //---------------------------------------------
  $relationship_cv = tripal_get_default_cv('biomaterial_relationship', 'type_id');
  $cv_id = $relationship_cv ? $relationship_cv->cv_id : NULL;
  $details = array(
    'relationship_table' => 'biomaterial_relationship', // the name of the _relationship table
    'base_table' => 'biomaterial',                      // the name of your chado base table
    'base_foreign_key' => 'biomaterial_id',             // the name of the key in your base chado table
    'base_key_value' => $biomaterial_id,                // the value of example_id for this record
    'nodetype' => 'biomaterial',                        // the human-readable name of your node type
    'cv_id' => $cv_id,                              // the cv.cv_id of the cv governing example_relationship.type_id
    'base_name_field' => 'name',                    // the base table field you want to be used as the name
    'subject_field_name' => 'subject_biomaterial_id',
    'object_field_name' => 'object_biomaterial_id',
    'select_options' => $select_options
  );
  // Adds the form elements to your current form
  chado_add_node_form_relationships($form, $form_state, $details);

  return $form;

}

/**
 * Implements hook_validate().
 * Validates submission of form when adding or updating a biomaterial node
 *
 * @ingroup tripal_biomaterial
 */
function chado_biomaterial_validate($node, $form, &$form_state) {

  // if this is a delete then don't validate
  if($node->op == 'Delete') {
    return;
  }

  // we are syncing if we do not have a node ID but we do have a biomaterial_id. We don't
  // need to validate during syncing so just skip it.
  if (is_null($node->nid) and property_exists($node, 'biomaterial_id') and $node->biomaterial_id != 0) {
    return;
  }

  // trim white space from text fields
  $node->title = trim($node->title);
  $node->description = trim($node->description);

  $biomaterial = 0;
  // check to make sure the name on the biomaterial is unique
  // before we try to insert into chado.
  if (property_exists($node, 'biomaterial_id')) {
    $sql = "SELECT * FROM {biomaterial} WHERE name = :name AND NOT biomaterial_id = :biomaterial_id";
    $biomaterial = chado_query($sql, array(':name' => $node->title, ':biomaterial_id' => $node->biomaterial_id))->fetchObject();
  }
  else {
    $sql = "SELECT * FROM {biomaterial} WHERE name = :name";
    $biomaterial = chado_query($sql, array(':name' => $node->title))->fetchObject();
  }
  if ($biomaterial) {
    form_set_error('title', t('The unique biomaterial name already exists. Please choose another'));
  }
}

/**
 * Implementation of hook_insert().
 *
 *  @parm $node
 *    Then node that has the information stored within, accessed given the nid
 *
 * @ingroup tripal_biomaterial
 */
function chado_biomaterial_insert($node) {

  $node->title = trim($node->title);
  $node->description = trim($node->description);

  // if there is an biomaterial_id in the $node object then this must be a sync so
  // we can skip adding the biomaterial as it is already there, although
  // we do need to proceed with the rest of the insert
  if (!property_exists($node, 'biomaterial_id')) {
    $values = array(
      'name' => $node->title,
      'description' => '',
    );
    $biomaterial = chado_insert_record('biomaterial', $values);
    if (!$biomaterial) {
      drupal_set_message(t('Unable to add biomaterial.', 'warning'));
      watchdog('tripal_biomaterial', 'Insert biomaterial: Unable to create biomaterial where values:%values',
      array('%values' => print_r($values, TRUE)), WATCHDOG_ERROR);
      return;
    }
    $biomaterial_id = $biomaterial['biomaterial_id'];

    // * Properties Form *
    // Add the description property
    $properties = chado_retrieve_node_form_properties($node);
    $descrip_id = tripal_get_cvterm(array(
      'name' => 'Biomaterial Description',
      'cv_id' => array('name' => 'biomaterial_property')
    ));
    $properties[$descrip_id->cvterm_id][0] = $node->description;

    $details = array(
      'property_table' => 'biomaterialprop',
      'base_table' => 'biomaterial',
      'foreignkey_name' => 'biomaterial_id',
      'foreignkey_value' => $biomaterial_id
    );
    chado_update_node_form_properties($node, $details, $properties);

    // * Relationships Form *
    $details = array(
      'relationship_table' => 'biomaterial_relationship',  // name of the _relationship table
      'foreignkey_value' => $biomaterial_id                // value of the example_id key
    );
    chado_update_node_form_relationships($node, $details);

  }
  else {
    $biomaterial_id = $node->biomaterial_id;
  }

  // Make sure the entry for this biomaterial doesn't already exist in the
  // chado_biomaterial table if it doesn't exist then we want to add it.
  $check_org_id = chado_get_id_from_nid('biomaterial', $node->nid);
  if (!$check_org_id) {
    $record = new stdClass();
    $record->nid = $node->nid;
    $record->vid = $node->vid;
    $record->biomaterial_id = $biomaterial_id;
    drupal_write_record('chado_biomaterial', $record);
  }
}

/**
 * Implementation of hook_delete().
 *
 * @param $node
 *  The node which is to be deleted, only chado biomaterial and chado_biomaterial need to be dealt with
 *  since the drupal node is deleted automagically
 *
 * @ingroup tripal_biomaterial
 */
function chado_biomaterial_delete($node) {

  $biomaterial_id = chado_get_id_from_nid('biomaterial', $node->nid);

  // if we don't have a biomaterial id for this node then this isn't a node of
  // type chado_biomaterial or the entry in the chado_biomaterial table was lost.
  if (!$biomaterial_id) {
    return;
  }

  // Remove data from {chado_biomaterial}, {node} and {node_revisions} tables of
  // drupal database
  $sql_del = "DELETE FROM {chado_biomaterial} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node_revision} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del,  array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del,  array(':nid' => $node->nid, ':vid' => $node->vid));

  // Remove data from biomaterial and biomaterialprop tables of chado database as well
  chado_query("DELETE FROM {biomaterialprop} WHERE biomaterial_id = :biomaterial_id", array(':biomaterial_id' => $biomaterial_id));
  chado_query("DELETE FROM {biomaterial} WHERE biomaterial_id = :biomaterial_id", array(':biomaterial_id' => $biomaterial_id));
}

/**
 * Implements hook_update().
 *
 * @param $node
 *  The node which is to have its containing information updated when the user modifies information
 *  pertaining to the specific biomaterial
 *
 * @ingroup tripal_biomaterial
 */
function chado_biomaterial_update($node) {

  $node->title = trim($node->title);
  $node->description = trim($node->description);

  // update the biomaterial and the description
  $biomaterial_id = chado_get_id_from_nid('biomaterial', $node->nid) ;
  $match = array('biomaterial_id' => $biomaterial_id);
  $values = array(
    'name' => $node->title,
    'description' => '',
  );
  $status = chado_update_record('biomaterial', $match, $values);
  if (!$status) {
    drupal_set_message(t('Unable to update biomaterial.', 'warning'));
    watchdog('tripal_biomaterial', 'Update biomaterial: Unable to update biomaterial where values: %values',
    array('%values' => print_r($values, TRUE)), WATCHDOG_ERROR);
  }

  // * Properties Form *
  // Add the description property
  $properties = chado_retrieve_node_form_properties($node);
  $descrip_id = tripal_get_cvterm(array(
    'name' => 'Biomaterial Description',
    'cv_id' => array('name' => 'biomaterial_property')
  ));
  $properties[$descrip_id->cvterm_id][0] = $node->description;

  $details = array(
    'property_table' => 'biomaterialprop',
    'base_table' => 'biomaterial',
    'foreignkey_name' => 'biomaterial_id',
    'foreignkey_value' => $biomaterial_id
  );
  chado_update_node_form_properties($node, $details, $properties);

  // * Relationships Form *
  $details = array(
    'relationship_table' => 'biomaterial_relationship',  // name of the _relationship table
    'foreignkey_value' => $biomaterial_id                // value of the example_id key
  );
  chado_update_node_form_relationships($node, $details);
}

/**
 * Implementation of hook_load().
 *
 * @param $node
 *   The node that is to have its containing information loaded
 *
 * @ingroup tripal_biomaterial
 */
function chado_biomaterial_load($nodes) {

  foreach ($nodes as $nid => $node) {
    // get the feature details from chado
    $biomaterial_id = chado_get_id_from_nid('biomaterial', $node->nid);

    // if the nid does not have a matching record then skip this node.
    // this can happen with orphaned nodes.
    if (!$biomaterial_id) {
      continue;
    }


    $values = array('biomaterial_id' => $biomaterial_id);
    $biomaterial = chado_generate_var('biomaterial', $values);

    $nodes[$nid]->biomaterial = $biomaterial;

    // Now get the title
    $node->title = chado_get_node_title($node);

  }

}

/**
 * Implement hook_node_access().
 *
 * This hook allows node modules to limit access to the node types they define.
 *
 *  @param $node
 *  The node on which the operation is to be performed, or, if it does not yet exist, the
 *  type of node to be created
 *
 *  @param $op
 *  The operation to be performed
 *
 *
 *  @param $account
 *  A user object representing the user for whom the operation is to be performed
 *
 *  @return
 *  If the permission for the specified operation is not set then return FALSE. If the
 *  permission is set then return NULL as this allows other modules to disable
 *  access.  The only exception is when the $op == 'create'.  We will always
 *  return TRUE if the permission is set.
 *
 * @ingroup tripal_biomaterial
 */
function chado_biomaterial_node_access($node, $op, $account) {

  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }

  if($node_type == 'chado_biomaterial') {
    if ($op == 'create') {
      if (!user_access('create chado_biomaterial content', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
    if ($op == 'update') {
      if (!user_access('edit chado_biomaterial content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete chado_biomaterial content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access chado_biomaterial content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    return NODE_ACCESS_IGNORE;
  }
}

/**
 * Implements hook_node_view().
 *
 * @ingroup tripal_biomaterial
 */
function tripal_biomaterial_node_view($node, $view_mode, $langcode) {
  switch ($node->type) {
    case 'chado_biomaterial':
      // Show feature browser and counts
      if ($view_mode == 'full') {
        $node->content['tripal_biomaterial_base'] = array(
          '#markup' => theme('tripal_biomaterial_base', array('node' => $node)),
          '#tripal_toc_id'    => 'base',
          '#tripal_toc_title' => 'Overview',
          '#weight' => -100,
        );
        $node->content['tripal_biomaterial_organism'] = array(
          '#markup' => theme('tripal_biomaterial_organism', array('node' => $node)),
          '#tripal_toc_id'    => 'organisms',
          '#tripal_toc_title' => 'Organisms',
        );
        $node->content['tripal_biomaterial_properties'] = array(
          '#markup' => theme('tripal_biomaterial_properties', array('node' => $node)),
          '#tripal_toc_id'    => 'properties',
          '#tripal_toc_title' => 'Properties',
        );
        $node->content['tripal_biomaterial_project'] = array(
          '#markup' => theme('tripal_biomaterial_project', array('node' => $node)),
          '#tripal_toc_id'    => 'projects',
          '#tripal_toc_title' => 'Projects',
        );
        $node->content['tripal_biomaterial_stock'] = array(
          '#markup' => theme('tripal_biomaterial_stock', array('node' => $node)),
          '#tripal_toc_id'    => 'stocks',
          '#tripal_toc_title' => 'Stocks',
        );
      }
      if ($view_mode == 'teaser') {
        $node->content['tripal_biomaterial_teaser'] = array(
          '#markup' => theme('tripal_biomaterial_teaser', array('node' => $node)),
        );
      }
      break;
  }
}

/**
 * Implements hook_node_insert().
 * Acts on all content types.
 *
 * @ingroup tripal_biomaterial
 */
function tripal_biomaterial_node_insert($node) {

  // set the URL path after inserting.  We do it here because we do not
  // know the biomaterial_id in the presave
  switch ($node->type) {
    case 'chado_biomaterial':

      // We still don't have a fully loaded node object in this hook. Therefore,
      // we need to simulate one so that the right values are available for
      // the URL to be determined.
      $biomaterial_id = chado_get_id_from_nid('biomaterial', $node->nid);
      $node->biomaterial = chado_generate_var('biomaterial', array('biomaterial_id' => $biomaterial_id));

      // Now get the title.
      $node->title = chado_get_node_title($node);

      // Now use the API to set the path.
      chado_set_node_url($node);

      break;
  }
}

/**
 * Implements hook_node_update().
 * Acts on all content types.
 *
 * @ingroup tripal_biomaterial
 */
function tripal_biomaterial_node_update($node) {

  // add items to other nodes, build index and search results
  switch ($node->type) {
    case 'chado_biomaterial':

      // Now get the title
      $node->title = chado_get_node_title($node);

      // Now use the API to set the path.
      chado_set_node_url($node);

      break;
  }
}

/**
 * Implements [content_type]_chado_node_default_title_format().
 *
 * Defines a default title format for the Chado Node API to set the titles on
 * Chado biomaterial nodes based on chado fields.
 */
function chado_biomaterial_chado_node_default_title_format() {
  return '[biomaterial.name]';
}

/**
 * Implements hook_chado_node_default_url_format().
 *
 * Designates a default URL format for biomaterial nodes.
 */
function chado_biomaterial_chado_node_default_url_format() {
  return '/biomaterial/[biomaterial.biomaterial_id]';
}
