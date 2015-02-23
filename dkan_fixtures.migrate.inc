<?php
/**
 * @file
 * Migration file for dkan_fixtures
 */


/**
 * Implements hook_migrate_api().
 */
function dkan_fixtures_migrate_api() {
  $api = array(
    // Migrate API, not CKAN's of course.
    'api' => 2,
    'groups' => array(
      'dkan_fixtures' => array(
        'title' => t('DKAN Fixtures'),
      ),
    ),
    'migrations' => array(
      'ckan_dataset_fixtures' => array(
        'class_name' => 'MigrateCkanDatasetImport',
        'group_name' => 'dkan_fixtures',
        'title' => t('DKAN Dataset Fixtures'),
      ),
      'ckan_resource_fixtures' => array(
        'class_name' => 'MigrateCkanResourceImport',
        'group_name' => 'dkan_fixtures',
        'title' => t('DKAN Resource Fixtures'),
      ),
    ),
  );
  return $api;
}

class MigrateCkanDatasetImport extends MigrateCkanDatasetBase {
  /**
   * Here we go.
   */
  public function __construct($arguments) {
    $arguments['endpoint'] = drupal_get_path('module', 'dkan_fixtures') . '/data/';
    $arguments['list_url'] = 'package_list.json';
    $arguments['item_url'] = 'package_show?id=:id.json';
    parent::__construct($arguments);

    $fields = $this->additionalFields();
    foreach ($fields as $id => $field) {
      $this->addFieldMapping($field, $field);
    }
  }

  /**
   * Gets additional fields.
   */
  public function additionalFields() {
    $api = open_data_schema_map_api_load('ckan_package_show');
    $fields = open_data_schema_map_additional_fields($api);
    $additional_fields = array();
    foreach ($fields as $id => $field) {
      if ($field != 'field_resources' &&
          $field != 'field_contact_email' &&
          $field != 'field_spatial' &&
          $field != 'field_tags' &&
          $field != 'field_additional_info') {
        $additional_fields[] = $field;
      }
    }
    return $additional_fields;
  }

  /**
   * Implements prepareRow.
   */
  public function prepareRow($row) {
    parent::prepareRow($row);
    $fields = $this->additionalFields();
    // Uses 'value' if available.
    foreach ($fields as $id => $field) {
      if (is_array($row->dkan_additional_fields->{$field}) && $row->dkan_additional_fields->{$field}) {
        if (is_object($row->dkan_additional_fields->{$field}[0])) {
          if (isset($row->dkan_additional_fields->{$field}[0]->value)) {
            $row->{$field} = $row->dkan_additional_fields->{$field}[0]->value;
          }
        }
        else {
          $row->{$field} = $row->dkan_additional_fields->{$field}[0];
        }
      }
      elseif (is_object($row->dkan_additional_fields->{$field})) {
        if (isset($row->dkan_additional_fields->{$field}->value)) {
          $row->{$field} = $row->dkan_additional_fields->{$field}->value;
        }
      }
      else {
        $row->{$field} = $row->dkan_additional_fields->{$field};
      }
    }
  }
}

class MigrateCkanResourceImport extends MigrateCkanResourceBase {
  /**
   * Here we go.
   */
  public function __construct($arguments) {
    $arguments['endpoint'] = drupal_get_path('module', 'dkan_fixtures') . '/data/';
    $arguments['list_url'] = 'resource_list.json';
    $arguments['item_url'] = 'resource_show?id=:id.json';
    parent::__construct($arguments);

  }

  /**
   * Implements prepareRow.
   */
  public function prepareRow($row) {
    parent::prepareRow($row);
    // Replace with absolute path.
    if (isset($row->url)) {
      $row->url = DRUPAL_ROOT . '/' . $this->endpoint . $row->url;
      // Create file.
      $data = file_get_contents($row->url);
      $name = basename($row->url);
      $uri = 'public://' . $name;
      $file = file_save_data($data, $uri, FILE_EXISTS_REPLACE);
      $row->file = $file->fid;
      // Unset other fields populated from url.
      unset($row->file_remote_link);
      unset($row->link);
    }
  }
}
