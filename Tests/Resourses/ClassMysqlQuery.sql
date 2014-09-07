-- Please dont chage query structure --

SELECT *
FROM employees;

SELECT emp_no, first_name, gender, hire_date
FROM employees;

SELECT *
FROM employees
WHERE emp_no <> 10001;

SELECT DISTINCT emp_no, first_name, gender, hire_date
FROM employees;

SELECT emp_no, first_name
FROM employees
WHERE emp_no <> 10001;

SELECT *
FROM employees
WHERE emp_no <> 10001 AND emp_no >= 10434;

SELECT *
FROM employees
WHERE emp_no <> 10001 OR emp_no >= 10434;

SELECT *
FROM employees
WHERE emp_no <> 10001 OR emp_no >= 10434 AND emp_no <> 10434;

SELECT *
FROM employees e
 JOIN titles t ON e.emp_no = t.emp_no;

SELECT *
FROM employees e
 LEFT JOIN titles t ON e.emp_no = t.emp_no;

SELECT *
FROM employees e
 RIGHT JOIN titles t ON e.emp_no = t.emp_no;

SELECT *
FROM employees e
 CROSS JOIN titles t;

SELECT *
FROM employees e
 NATURAL JOIN titles t;

SELECT *
FROM employees e
 JOIN titles t ON e.emp_no = t.emp_no
WHERE e.emp_no < 10434;


SELECT count(emp_no) as empno, first_name
FROM employees
WHERE emp_no <> 10001
GROUP BY first_name;

SELECT *
FROM salaries
WHERE emp_no >= 10001
HAVING sum(salary) > 1000;

SELECT emp_no
FROM employees
UNION (SELECT emp_no FROM salaries);

SELECT emp_no, first_name
FROM employees
WHERE emp_no <> 10001
ORDER BY first_name;

SELECT emp_no, first_name
FROM employees LIMIT 10;

SELECT emp_no, first_name
FROM employees LIMIT 10 OFFSET 10;

-- Known Issue
SELECT emp_no, first_name
FROM employees OFFSET 10;