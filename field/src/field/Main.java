package field;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

public class Main {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		Example example = new Example();
		Class exampleClass = example.getClass();
		Field[] fields = exampleClass.getDeclaredFields();
		for(int i=0; i<fields.length; i++) {
			Field field = fields[i];
			System.out.println("name:"+field.getName());
			Class type = field.getType();
			System.out.println("type:"+type);
			boolean isTurn = true;
			while(isTurn) {
				isTurn=false;
				try {
					System.out.println("old value:"+field.get(example));
					if(type.toString().equals("int")) {
						field.setInt(example, 10);

					}else if(type.toString().equals("float")) {
						field.setFloat(example, 20.0F);

					}else if(type.toString().equals("boolean")) {
						field.setBoolean(example, true);

					}else {
						field.set(example,"hellojava");
					}
					System.out.println("new value:"+field.get(example));
				}catch (Exception e) {
					//					e.printStackTrace();
					isTurn = true;
					System.out.println("exception occured");
					field.setAccessible(true);
				}

			}
		}

		methode methode = new methode();
		Class methodC = methode.getClass();
		Method[] methods = methodC.getDeclaredMethods();
		for(int i=0; i<methods.length; i++) {
			Method method = methods[i];
			System.out.println("name:"+method.getName());
			System.out.println("var arg:"+method.isVarArgs());
			System.out.println("param");
			Class[] paramTypes = method.getParameterTypes();
			for(int j=0; j<paramTypes.length; j++) {
				System.out.println("type:"+j+paramTypes[j]);
			}
			System.out.println("return type:"+method.getReturnType());
			Class[] exceptionTypes = method.getExceptionTypes();
			for(int k=0; k<exceptionTypes.length; k++) {
				System.out.println("exception type:"+exceptionTypes[k]);
			}
			boolean isTurn = true;
			while(isTurn) {
				
				try {
					isTurn = false;
					if("staticMethod".equals(method.getName())) {
						method.invoke(methode);
					}else if("publicMethod".equals(method.getName())) {
						method.invoke(methode, 10);
					}else if("protectedMethod".equals(method.getName())) {
						method.invoke(methode, "10",10);
					}else {
						Object[] param = new Object[] {new String[] {"ding","le","wangqianlan"}};
						method.invoke(methode, param);
					}
				}catch (Exception e) {
					System.out.println("exception");
					method.setAccessible(true);
					isTurn = true;
				}
			}
		}
	}

}
