<?php 

/**
 * @file os_migrate.admin.inc
 * 
 * Provides form for OpenScholar migrate settings
 */

/**
 * os_migrate's admin form
 */
function os_migrate_admin() {
  $form = array();
  $form['os_migrate_database'] = array(
    '#title' => t('Migration database'),
    '#description' => t('Drupal 6 database from which to migrate data.'),
    '#type' => 'textfield',
    '#default_value' => variable_get('os_migrate_database', ''),
  );

  $form['#validate'][] = 'os_migrate_admin_validate';
  
  $form['settings_help'] = array(
    '#title' => t('settings.php configuration'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#type' => 'fieldset',
    '#description' => <<<EOF
<p>Your settings.php file needs to define a connection to the Drupal 6 database.  Please ensure that
in addition to the default connection to your Drupal 7 database, settings.php includes a connection
to the database you specifed.  It will look something like the text below, but with the bold values
replaced with those applicable to your DB.  </p>

The Migration database setting on this page corresponds to the <strong>database</strong> entry below 
as specified in your settings.php file.

<pre>
\$databases['<strong>database</strong>']['default'] = array(
  'database' => '<strong>db name</strong>',
  'username' => '<strong>db user</strong>',
  'password' => '<strong>db password</strong>',
  'host' => 'localhost',
  'port' => '', 
  'driver' => 'mysql',
  'prefix' => '', 
);
</pre>
EOF
  );

  $form['os_migrate_files'] = array(
    '#title' => t('Files directory'),
    '#description' => t('Path to your Drupal 6 installation\'s files.  Migrate will use this path for copying uploaded files (e.g. /var/www/openscholar/)'),
    '#type' => 'textfield',
    '#default_value' => variable_get('os_migrate_files', ''),
  );
  
  $form['dev_fieldset'] = array(
    '#title' => t('Development Settings'),
    '#type' => 'fieldset',
    '#collapsed' => FALSE,
    '#collapsible' => TRUE,
  );
  
  $form['dev_fieldset']['os_migrate_email_notification'] = array(
    '#title' => t('Email Notification'),
    '#description' => t('Creating a user account during migration will send that user an email.  
    										When this is checked, user accounts will be created with the email address
    										in the legacy database.  Leave this unchecked during development to munge
    										the email addresses to avoid spamming all the users.'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('os_migrate_email_notification', FALSE),
  );
  
  $form['dev_fieldset']['os_migrate_ignore_dependencies'] = array(
    '#title' => t('Ignore Dependencies'),
    '#description' => t('Migration dependencies will be ignored by default.'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('os_migrate_ignore_dependencies', FALSE),
  );
  
  $form['dev_fieldset']['os_migrate_force_og'] = array(
    '#title' => t('Force Group ID'),
    '#description' => t('Force all content into group.'),
    '#type' => 'textfield',
    '#default_value' => variable_get('os_migrate_force_og', FALSE),
  );
  return system_settings_form($form);
}

/**
 * Validates os_migrate's admin form.
 * Tries to connect to supplied db.  
 */
function os_migrate_admin_validate($form, &$form_state) {
  $os_migrate_database = $form_state['values']['os_migrate_database'];
  $os_migrate_files = $form_state['values']['os_migrate_files'];
  
  // try to connect to database to confirm it's set up correctly.
  if (strlen($os_migrate_database)) {
    try {
      Database::getConnection('default', $os_migrate_database);
      Database::closeConnection('default', $os_migrate_database);
      drupal_set_message('Connected to migration database.');
    } catch (DatabaseConnectionNotDefinedException $e){
      form_set_error('os_migrate_database', t('Unable to connect to specified database.  Check your settings.php configuration.'));
    }
  }  
  
  // check that files dir exists
  if (strlen($os_migrate_files)) {
    if (($dh = @opendir($os_migrate_files)) != FALSE) {
      closedir($dh);
      drupal_set_message('Files directory exists.');
    } else {
      form_set_error('os_migrate_files', t('Unable to open files directory.  Please check that it exists and is writeable by the webserver.'));
    }
  }
}