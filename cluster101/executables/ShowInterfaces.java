import java.net.*; import java.util.*; public class ShowInterfaces {
	public static void main(String[] args) throws Exception {
		NetworkInterface ni = NetworkInterface.getByName("eth0");
		Enumeration<InetAddress> inetAddresses = ni.getInetAddresses();
		for (InetAddress inetAddress : Collections.list(inetAddresses)) {
			if(inetAddress instanceof Inet4Address) {
				System.out.printf("Inet4Address: %s\n", ((Inet4Address)inetAddress).getHostAddress());
			}
		}
	}
}
