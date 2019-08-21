package field;

public class methode {
	static void  staticMethod() {
		System.out.println("staticMethod");
	}

	public int publicMethod(int i) {
		System.out.println("publicMethod");
		return 10 *i;
	}
	
	protected int protectedMethod(String s, int i) throws NumberFormatException{
		System.out.println("protectedMetho");
		return Integer.valueOf(s) + i;
	}
	
	private String privateMethod(String ...strings) {
		System.out.println("privateMethod");
		StringBuffer strbuf = new StringBuffer();
		for(int i=0; i<strings.length; i++) {
			strbuf.append(strings[i]);
		}
		return strbuf.toString();
	}
}
