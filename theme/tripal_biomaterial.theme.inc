<?php
/**
 *
 *
 * @ingroup tripal_biomaterial
 */
function tripal_biomaterial_preprocess_tripal_biomaterial_relationships(&$variables) {
  $biomaterial = $variables['node']->biomaterial;

  // expand the biomaterial object to include the biomaterial relationships.
  $options = array(
    'return_array' => 1,
    // we don't want to fully recurse we only need information about the
    // relationship type and the object and subject biomaterials
    'include_fk' => array(
      'type_id'    => 1,
      'object_biomaterial_id'  => 1,
      'subject_biomaterial_id' => 1,
    ),
  );
  $biomaterial = chado_expand_var($biomaterial, 'table', 'biomaterial_relationship', $options);

  // get the subject relationships
  $srelationships = $biomaterial->biomaterial_relationship->subject_biomaterial_id;
  $orelationships = $biomaterial->biomaterial_relationship->object_biomaterial_id;

  // combine both object and subject relationshisp into a single array
  $relationships = array();
  $relationships['object'] = array();
  $relationships['subject'] = array();

  // iterate through the object relationships
  if ($orelationships) {
    foreach ($orelationships as $relationship) {
       $rel = new stdClass();
       $rel->record = $relationship;

       // get the relationship and child types
       $rel_type = t(preg_replace('/_/', " ", $relationship->type_id->name));

       // get the node id of the subject
       $sql = "SELECT nid FROM {chado_biomaterial} WHERE biomaterial_id = :biomaterial_id";
       $n = db_query($sql, array(':biomaterial_id' => $relationship->subject_biomaterial_id->biomaterial_id))->fetchObject();
       if ($n) {
          $rel->record->nid = $n->nid;
       }

       $relationships['object'][$rel_type][] = $rel;
    }
  }

  // now add in the subject relationships
  if ($srelationships) {
    foreach ($srelationships as $relationship) {
       $rel = new stdClass();
       $rel->record = $relationship;

       $rel_type = t(preg_replace('/_/', " ", $relationship->type_id->name));

       // get the node id of the subject
       $sql = "SELECT nid FROM {chado_biomaterial} WHERE biomaterial_id = :biomaterial_id";
       $n = db_query($sql, array(':biomaterial_id' => $relationship->object_biomaterial_id->biomaterial_id))->fetchObject();
       if ($n) {
          $rel->record->nid = $n->nid;
       }

       $relationships['subject'][$rel_type][] = $rel;
    }
  }
  $biomaterial->all_relationships = $relationships;
}
