<?php
/**
 * @file
 * Administration of biomaterials
 */

/**
 * Admin launchpad
 *
 * @ingroup tripal_biomaterial
 */
function tripal_biomaterial_admin_biomaterial_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/chado');
  $breadcrumb[] = l('Biomaterials', 'admin/tripal/chado/tripal_biomaterial');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_biomaterial_admin_biomaterials','default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Biomaterial module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
      $output .= '<li>'.l('Biomaterials View', 'admin/tripal/chado/tripal_biomaterial/views/biomaterials/enable').'</li>';
    $output .= '</ul>';
  }

  return $output;
}
/**
 * Biomaterial settings
 *
 * @ingroup tripal_biomaterial
 */
function tripal_biomaterial_admin($form, $form_state) {
  $form = array();

  // If your module is using the Chado Node: Title & Path API to allow custom titles
  // for your node type then you need to add the configuration form for this functionality.
  $details = array(
    'module' => 'tripal_biomaterial',       // the name of the MODULE implementing the content type
    'content_type' => 'chado_biomaterial',   // the name of the content type
      // An array of options to use under "Page Titles"
      // the key should be the token and the value should be the human-readable option
    'options' => array(
      '[biomaterial.name]' => 'biomaterial Name Only',
        // there should always be one options matching the unique constraint.
      '[biomaterial.name]' => 'Unique Contraint: The biomaterial name'
    ),
    // the token indicating the unique constraint in the options array
    'unique_option' => '[biomaterial.name]'
  );
  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_title($form, $form_state, $details);

  // If the module is using the "Chado Node: Title & Path API" to allow custom URLs
  // for your node type then you need to add the configuration form for this functionality.
  $details = array(
    'module' => 'tripal_biomaterial',        // the name of the MODULE implementing the content type
    'content_type' => 'chado_biomaterial',   // the name of the content type
      // An array of options to use under "Page URLs"
      // the key should be the token and the value should be the human-readable option
    'options' => array(
      '/biomaterial/[biomaterial.name]' => 'Biomaterial Name Only',
        // there should always be one options matching the unique constraint.
        // If you have a more human-readable constraint, then that is preferrable.
        // See the tripal feature module for a good example of this.
      '/biomaterial/[biomaterial.biomaterial_id]' => 'Unique Contraint: The Biomaterial ID'
    ),
  );
  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_url($form, $form_state, $details);

  return system_settings_form($form);
}
