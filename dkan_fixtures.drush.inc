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
