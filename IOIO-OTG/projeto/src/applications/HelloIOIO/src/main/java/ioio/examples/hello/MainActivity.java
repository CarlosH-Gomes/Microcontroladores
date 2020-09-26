package ioio.examples.hello;

import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IOIO;
import ioio.lib.api.IOIO.VersionType;
import ioio.lib.api.PwmOutput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.BaseIOIOLooper;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.android.IOIOActivity;
import android.content.Context;
import android.os.Bundle;
import android.widget.Button;
import android.widget.Toast;
import android.widget.ToggleButton;

/**
 * This is the main activity of the HelloIOIO example application.
 *
 * It displays a toggle button on the screen, which enables control of the
 * on-board LED. This example shows a very simple usage of the IOIO, by using
 * the {@link IOIOActivity} class. For a more advanced use case, see the
 * HelloIOIOPower example.
 */
public class MainActivity extends IOIOActivity {
	private Button frente;
	private Button tras;
	private Button direita;
	private Button esquerda;

	/**
	 * Called when the activity is first created. Here we normally initialize
	 * our GUI.
	 */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		frente = (Button) findViewById(R.id.btn_frente);
		tras = (Button) findViewById(R.id.btn_tras);
		direita = (Button) findViewById(R.id.btn_direita);
		esquerda = (Button) findViewById(R.id.btn_esquerda);
	}

	/**
	 * This is the thread on which all the IOIO activity happens. It will be run
	 * every time the application is resumed and aborted when it is paused. The
	 * method setup() will be called right after a connection with the IOIO has
	 * been established (which might happen several times!). Then, loop() will
	 * be called repetitively until the IOIO gets disconnected.
	 */
	class Looper extends BaseIOIOLooper {
		/** The on-board LED. */
		private DigitalOutput pin_1_motor1;
		private DigitalOutput pin_2_motor1;
		private DigitalOutput pin_3_motor2;
		private DigitalOutput pin_4_motor2;
		private PwmOutput motor1;
		private PwmOutput motor2;

		/**
		 * Called every time a connection with IOIO has been established.
		 * Typically used to open pins.
		 *
		 * @throws ConnectionLostException
		 *             When IOIO connection is lost.
		 *
		 */
		@Override
		protected void setup() throws ConnectionLostException {
			showVersions(ioio_, "IOIO connected!");
			pin_1_motor1 = ioio_.openDigitalOutput(1, false);
			pin_2_motor1 = ioio_.openDigitalOutput(2, false);
			pin_3_motor2 = ioio_.openDigitalOutput(3, false);
			pin_4_motor2 = ioio_.openDigitalOutput(4, false);
			motor1 = ioio_.openPwmOutput(5, 100);
			motor2 = ioio_.openPwmOutput(6, 100);
		}

		/**
		 * Called repetitively while the IOIO is connected.
		 *
		 * @throws ConnectionLostException
		 *             When IOIO connection is lost.
		 * @throws InterruptedException
		 * 				When the IOIO thread has been interrupted.
		 *
		 * @see ioio.lib.util.IOIOLooper#loop()
		 */
		@Override
		public void loop() throws ConnectionLostException, InterruptedException {
			if(frente.isPressed()){
				pin_1_motor1.write(true);
				pin_2_motor1.write(false);
				pin_3_motor2.write(true);
				pin_4_motor2.write(false);

				motor1.setDutyCycle(1.0f);
				motor2.setDutyCycle(1.0f);
			}
			else if(tras.isPressed()){
				pin_1_motor1.write(false);
				pin_2_motor1.write(true);
				pin_3_motor2.write(false);
				pin_4_motor2.write(true);

				motor1.setDutyCycle(1.0f);
				motor2.setDutyCycle(1.0f);
			}
			else if(direita.isPressed()){
				pin_1_motor1.write(true);
				pin_2_motor1.write(false);
				pin_3_motor2.write(true);
				pin_4_motor2.write(false);

				motor1.setDutyCycle(0.5f);
				motor2.setDutyCycle(1.0f);
			}
			else if(esquerda.isPressed()){
				pin_1_motor1.write(true);
				pin_2_motor1.write(false);
				pin_3_motor2.write(true);
				pin_4_motor2.write(false);

				motor1.setDutyCycle(1.0f);
				motor2.setDutyCycle(0.5f);
			}
			else if(esquerda.isPressed()){
				pin_1_motor1.write(false);
				pin_2_motor1.write(false);
				pin_3_motor2.write(false);
				pin_4_motor2.write(false);

				motor1.setDutyCycle(0.0f);
				motor2.setDutyCycle(0.0f);
			}

			Thread.sleep(100);
		}

		/**
		 * Called when the IOIO is disconnected.
		 *
		 * @see ioio.lib.util.IOIOLooper#disconnected()
		 */
		@Override
		public void disconnected() {

			toast("IOIO disconnected");
		}

		/**
		 * Called when the IOIO is connected, but has an incompatible firmware version.
		 *
		 * @see ioio.lib.util.IOIOLooper#incompatible(IOIO)
		 */
		@Override
		public void incompatible() {
			showVersions(ioio_, "Incompatible firmware version!");
		}
}


	@Override
	protected IOIOLooper createIOIOLooper() {
		return new Looper();
	}

	private void showVersions(IOIO ioio, String title) {
		toast(String.format("%s\n" +
				"IOIOLib: %s\n" +
				"Application firmware: %s\n" +
				"Bootloader firmware: %s\n" +
				"Hardware: %s",
				title,
				ioio.getImplVersion(VersionType.IOIOLIB_VER),
				ioio.getImplVersion(VersionType.APP_FIRMWARE_VER),
				ioio.getImplVersion(VersionType.BOOTLOADER_VER),
				ioio.getImplVersion(VersionType.HARDWARE_VER)));
	}

	private void toast(final String message) {
		final Context context = this;
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				Toast.makeText(context, message, Toast.LENGTH_LONG).show();
			}
		});
	}

	private int numConnected_ = 0;


}