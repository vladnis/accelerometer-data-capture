package com.disertatie.vlad.datacapture;

import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Environment;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import com.androidplot.xy.BoundaryMode;
import com.androidplot.xy.LineAndPointFormatter;
import com.androidplot.xy.SimpleXYSeries;
import com.androidplot.xy.XYPlot;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class CaptureActivity extends AppCompatActivity implements SensorEventListener {
    static final String SCENARIO_TAG = "com.disertatie.vlad.SCENARIO";
    private double[] gravity = new double[3];
    private double[] linear_acceleration = new double[3];

    private SensorManager mSensorManager;
    private Sensor mAccelerometer;

    private long mSensorTimeStamp;
    private long mCpuTimeStamp;

    FileOutputStream outputStream;

    private XYPlot plot;

    private int scenario = -1;

    private static final String TAG = "MyActivity";

    private SimpleXYSeries OZSeries = null;
    private SimpleXYSeries OXSeries = null;
    private SimpleXYSeries OYSeries = null;


    private static final int HISTORY_SIZE = 100;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_capture);
        Intent intent = getIntent();

        this.scenario = intent.getIntExtra(CaptureActivity.SCENARIO_TAG, -1);

        if (this.scenario == -1) {
            Snackbar.make(findViewById(R.id.captureCoordinatorLayout), R.string.selector_fail,
                    Snackbar.LENGTH_SHORT)
                    .show();
        }

        mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);

        // initialize our XYPlot reference:
        plot = (XYPlot) findViewById(R.id.plot);
        plot.setRangeBoundaries(-50, 50, BoundaryMode.FIXED);
        OZSeries = new SimpleXYSeries("OZ");
        OZSeries.useImplicitXVals();

        OXSeries = new SimpleXYSeries("OX");
        OXSeries.useImplicitXVals();

        OYSeries = new SimpleXYSeries("OY");
        OYSeries.useImplicitXVals();

        plot.addSeries(OZSeries, new LineAndPointFormatter(Color.GREEN, Color.GREEN, Color.TRANSPARENT, null));
        plot.addSeries(OXSeries, new LineAndPointFormatter(Color.YELLOW, Color.YELLOW, Color.TRANSPARENT, null));
        plot.addSeries(OYSeries, new LineAndPointFormatter(Color.BLUE, Color.BLUE, Color.TRANSPARENT, null));

    }

    public void onClickStartCapture(View view) {

        if (!isExternalStorageReadable() || !isExternalStorageWritable()) {
            Snackbar.make(findViewById(R.id.captureCoordinatorLayout), "External mount failed",
                    Snackbar.LENGTH_SHORT)
                    .show();
            return;
        }

        mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        final long now = mSensorTimeStamp + (System.nanoTime() - mCpuTimeStamp);

        String filename = MainActivity.scenariosFilenames[this.scenario].toString() + now;

        File file = new File(this.getExternalFilesDir(Environment.DIRECTORY_DCIM), filename);

        Log.d("test", file.getPath());

        try {
            outputStream = new FileOutputStream(file);
        } catch (Exception e) {
            e.printStackTrace();
        }

        mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_UI);

        Snackbar.make(findViewById(R.id.captureCoordinatorLayout), filename,
                1)
                .show();
    }


    public void onClickStopCapture(View view) {

        try {
            outputStream.flush();
            outputStream.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

        mSensorManager.unregisterListener(this);

        Snackbar.make(findViewById(R.id.captureCoordinatorLayout), "Finished capture",
                1)
                .show();

    }

    public void onSensorChanged(SensorEvent event){
        if (event.sensor.getType() != Sensor.TYPE_ACCELEROMETER)
            return;

        final double alpha = 0.8;

        // Isolate the force of gravity with the low-pass filter.
        gravity[0] = alpha * gravity[0] + (1 - alpha) * event.values[0]; // Y
        gravity[1] = alpha * gravity[1] + (1 - alpha) * event.values[1]; // X
        gravity[2] = alpha * gravity[2] + (1 - alpha) * event.values[2]; // Z

        // Remove the gravity contribution with the high-pass filter.
        linear_acceleration[0] = event.values[0] - gravity[0];
        linear_acceleration[1] = event.values[1] - gravity[1];
        linear_acceleration[2] = event.values[2] - gravity[2];

        mSensorTimeStamp = event.timestamp;
        mCpuTimeStamp = System.nanoTime();

        final long now = mSensorTimeStamp + (System.nanoTime() - mCpuTimeStamp);

        String line = linear_acceleration[0] + " " + linear_acceleration[1] + " " + linear_acceleration[2] + " " + now + "\n";

        try {
            outputStream.write(line.getBytes());
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (OZSeries.size() > HISTORY_SIZE) {
            OZSeries.removeFirst();
        }


        if (OXSeries.size() > HISTORY_SIZE) {
            OXSeries.removeFirst();
        }


        if (OYSeries.size() > HISTORY_SIZE) {
            OYSeries.removeFirst();
        }

        OZSeries.addLast(null, linear_acceleration[0]);
        OYSeries.addLast(null, linear_acceleration[2]);
        OXSeries.addLast(null, linear_acceleration[1]);

        plot.redraw();

    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i) {

    }

    /* Checks if external storage is available for read and write */
    public boolean isExternalStorageWritable() {
        String state = Environment.getExternalStorageState();
        if (Environment.MEDIA_MOUNTED.equals(state)) {
            return true;
        }
        return false;
    }

    /* Checks if external storage is available to at least read */
    public boolean isExternalStorageReadable() {
        String state = Environment.getExternalStorageState();
        if (Environment.MEDIA_MOUNTED.equals(state) ||
                Environment.MEDIA_MOUNTED_READ_ONLY.equals(state)) {
            return true;
        }
        return false;
    }

}
