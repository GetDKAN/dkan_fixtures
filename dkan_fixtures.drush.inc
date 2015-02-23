<?php
/**
 * @file
 * Drush integration for the dkan_fixtures module.
 */

/**
 * Implements hook_drush_command().
 */
function dkan_fixtures_drush_command() {
  $items = array();
  $items['dkan-save-data'] = array(
    'description' => dt('Creates JSON files out of dkan api endpoints'),
    'arguments' => array(),
    'aliases' => array('dsd'),
  );
  return $items;
}

/**
 * Drush save_fixtures command callback.
 */
function drush_dkan_fixtures_dkan_save_data() {
  // Allows other modules to receive exports.
  $modules = module_implements('dkan_fixtures_register');
  $modules = $modules ? $modules : array('dkan_fixtures');
  foreach ($modules as $module) {
    $module_path = drupal_get_path('module', $module);
    // Remove old files.
    $files = glob($module_path . '/data/*');
    foreach ($files as $file) {
      if (is_file($file)) {
        unlink($file);
      }
    }
    // Create new fixtures.
    $fixtures = dkan_fixtures_render_fixtures();
    // Save them.
    foreach ($fixtures as $fixture) {
      dkan_fixtures_save_fixture($fixture, $module_path);
    }
  }
}
