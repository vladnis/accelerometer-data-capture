package com.disertatie.vlad.datacapture;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

public class MainActivity extends AppCompatActivity {
    public static CharSequence scenarios[] = new CharSequence[] {"Stairs up", "Stairs down", "Elevator up", "Elevator down"};
    public static CharSequence scenariosFilenames[] = new CharSequence[] {"stairs-up", "stairs-down", "elevator-up", "elevator-down"};


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void onClickNewCapture(View view) {

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Pick scenario");
        builder.setItems(MainActivity.scenarios, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                Intent intent = new Intent();
                intent.putExtra(CaptureActivity.SCENARIO_TAG, which);
                startActivity(intent);
            }
        });
        builder.show();
    }

}
