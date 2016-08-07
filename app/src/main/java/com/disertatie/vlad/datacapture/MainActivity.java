package com.disertatie.vlad.datacapture;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ListActivity;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class MainActivity extends Activity {
    public static CharSequence scenarios[] = new CharSequence[] {"Stairs up", "Stairs down", "Elevator up", "Elevator down", "Walking"};
    public static CharSequence scenariosFilenames[] = new CharSequence[] {"stairs-up", "stairs-down", "elevator-up", "elevator-down", "walking"};
    ArrayAdapter<String> adapter;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    @Override
    public void onResume() {
        super.onResume();

        // gets the files in the directory
        File fileDirectory = new File(this.getExternalFilesDir(Environment.DIRECTORY_DCIM), "");

        // lists all the files into an array
        File[] dirFiles = fileDirectory.listFiles();

        if (dirFiles == null) {
            Log.e("test", "Failed to open data folder");
            return;
        }

        ArrayList<String> myStringArray = new ArrayList<String>();

        int pos = 0;
        // loops through the array of files, outputing the name to console
        for (File dirFile : dirFiles) {
            String fileOutput = dirFile.getName();
            myStringArray.add(fileOutput);
            pos++;
        }

        adapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_list_item_1, myStringArray);

        final ListView listView = (ListView) findViewById(R.id.file_list);
        listView.setAdapter(adapter);

        listView.setClickable(true);
        listView.setOnItemClickListener(new OnItemClickListener() {

            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, final int i, long l) {

                DialogInterface.OnClickListener dialogClickListener = new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        switch (which){
                            case DialogInterface.BUTTON_POSITIVE:
                                String filename = (String) (listView.getItemAtPosition(i));

                                String toRemove = adapter.getItem(i);

                                Log.d("tes", toRemove);

                                adapter.remove(toRemove);

                                File file = new File(MainActivity.this.getExternalFilesDir(Environment.DIRECTORY_DCIM), filename);
                                boolean delete = file.delete();

                                if (!delete) {
                                    Log.e("test", "Could not delete file");
                                }
                                break;

                            case DialogInterface.BUTTON_NEGATIVE:
                                //No button clicked
                                break;
                        }
                    }
                };


                AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
                builder.setMessage("Are you sure you want to delete file?").setPositiveButton("Yes", dialogClickListener)
                        .setNegativeButton("No", dialogClickListener).show();

            }

        });
    }

    public void onClickNewCapture(View view) {

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Pick scenario");
        builder.setItems(MainActivity.scenarios, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                Intent intent = new Intent(MainActivity.this, CaptureActivity.class);
                intent.putExtra(CaptureActivity.SCENARIO_TAG, which);
                startActivity(intent);
            }
        });
        builder.show();
    }

}
