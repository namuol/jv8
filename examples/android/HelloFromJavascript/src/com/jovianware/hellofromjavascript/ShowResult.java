package com.jovianware.hellofromjavascript;

import com.jovianware.jv8.V8Runner;
import com.jovianware.jv8.V8Value;

import android.os.Bundle;
import android.app.Activity;
import android.view.MenuItem;
import android.widget.TextView;
import android.support.v4.app.NavUtils;
import android.content.Intent;

public class ShowResult extends Activity {
  
  @Override
  public void onCreate(Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);

      // Get the message from the intent
      Intent intent = getIntent();
      String message = intent.getStringExtra(MainActivity.EXTRA_MESSAGE);

      // Create the text view
      TextView textView = new TextView(this);
      V8Runner v8 = new V8Runner();
      //V8Value result = v8.runJS("'Hello from JavaScript, " + message + "!'");
      V8Value result = v8.runJS("undefined");
      
      textView.setTextSize(40);
      textView.setText(result.toString());

      // Set the text view as the activity layout
      setContentView(textView);
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
      switch (item.getItemId()) {
      case android.R.id.home:
          NavUtils.navigateUpFromSameTask(this);
          return true;
      }
      return super.onOptionsItemSelected(item);
  }

}
